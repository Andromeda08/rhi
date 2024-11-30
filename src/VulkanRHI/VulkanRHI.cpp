#include "VulkanRHI.hpp"

#include "Platform.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanRenderPass.hpp"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VulkanRHI::VulkanRHI(const VulkanRHICreateInfo& createInfo)
: DynamicRHI()
, mWindow(createInfo.pWindow)
{
    const vk::DynamicLoader dynamicLoader;
    const auto vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    createInstance();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance);

    createDebugContext();

    createDevice();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(mDevice->handle());

    mSwapchain = VulkanSwapchain::createVulkanSwapchain({
        .pWindow = mWindow,
        .pDevice = mDevice.get(),
        .instance = mInstance,
        .imageCount = createInfo.backBufferCount,
    });

    vk::Result result;
    mImageReady.resize(mFramesInFlight);
    mRenderingFinished.resize(mFramesInFlight);
    mFrameInFlight.resize(mFramesInFlight);

    for (uint32_t i = 0; i < mFramesInFlight; i++)
    {
        auto semaphoreCreateInfo = vk::SemaphoreCreateInfo();
        auto fenceCreateInfo = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

        mImageReady[i] = mDevice->handle().createSemaphore(semaphoreCreateInfo);
        mRenderingFinished[i] = mDevice->handle().createSemaphore(semaphoreCreateInfo);
        mFrameInFlight[i] = mDevice->handle().createFence(fenceCreateInfo);
    }

    VK_PRINTLN(fmt::format("{} RHI initialized", VK_STYLED_PREFIX));
}

std::unique_ptr<VulkanRHI> VulkanRHI::createVulkanRHI(const VulkanRHICreateInfo& createInfo)
{
    return std::make_unique<VulkanRHI>(createInfo);
}

Frame VulkanRHI::beginFrame(const RHIFrameBeginInfo& frameBeginInfo)
{
    const vk::Fence fence = mFrameInFlight[mCurrentFrame];

    vk::Result result = mDevice->handle().waitForFences(1, &fence, true, std::numeric_limits<uint64_t>::max());
    result = mDevice->handle().resetFences(1, &fence);

    const auto nextImage = mDevice->handle().acquireNextImageKHR(
        mSwapchain->handle(),std::numeric_limits<uint64_t>::max(),
        mImageReady[mCurrentFrame], nullptr).value;

    return {
        .mCurrentFrame = mCurrentFrame,
        .mAcquiredFrameIndex = nextImage,
    };
}

void VulkanRHI::submitFrame(const Frame& frame)
{
    const auto frameIndex = frame.getCurrentFrame();

    std::vector<vk::CommandBufferSubmitInfo> commandBufferSubmitInfos;
    for (const auto commandBuffer : frame.mCommandLists)
    {
        const auto info = vk::CommandBufferSubmitInfo()
            .setCommandBuffer(commandBuffer->as<VulkanCommandList>()->handle());
        commandBufferSubmitInfos.push_back(info);
    }

    const auto waitSemaphoreInfo = vk::SemaphoreSubmitInfo()
        .setSemaphore(mImageReady[frameIndex])
        .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    std::vector waitSemaphoreInfos = { waitSemaphoreInfo };

    const auto signalSemaphoreInfo = vk::SemaphoreSubmitInfo()
        .setSemaphore(mRenderingFinished[frameIndex])
        .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    std::vector signalSemaphoreInfos = { signalSemaphoreInfo };

    const auto submitInfo = vk::SubmitInfo2()
        .setCommandBufferInfos(commandBufferSubmitInfos)
        .setCommandBufferInfoCount(commandBufferSubmitInfos.size())
        .setWaitSemaphoreInfos(waitSemaphoreInfos)
        .setWaitSemaphoreInfoCount(waitSemaphoreInfos.size())
        .setSignalSemaphoreInfos(signalSemaphoreInfos)
        .setSignalSemaphoreInfoCount(signalSemaphoreInfos.size());

    if (auto result = mDevice->getGraphicsQueue()->getQueue().submit2(1, &submitInfo, mFrameInFlight[frameIndex]);
        result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to submit CommandList");
    }

    mSwapchain->present(mRenderingFinished[frameIndex], frame.getAcquiredFrameIndex());

    mDevice->waitIdle();
    mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
}

std::unique_ptr<RHIBuffer> VulkanRHI::createBuffer(const RHIBufferCreateInfo& createInfo)
{
    if (createInfo.pData)
    {
        std::unique_ptr<VulkanBuffer> result;
        getGraphicsQueue()->executeSingleTimeCommand([&](RHICommandList* commandList) {
            result = VulkanBuffer::createVulkanBufferWithData({
                    .bufferSize = createInfo.bufferSize,
                    .bufferType = createInfo.bufferType,
                    .pDevice    = mDevice.get(),
                    .debugName  = createInfo.debugName,
                },
                createInfo.pData,
                commandList
            );
        });
        return result;
    }

    return VulkanBuffer::createVulkanBuffer({
        .bufferSize = createInfo.bufferSize,
        .bufferType = createInfo.bufferType,
        .pDevice    = mDevice.get(),
        .debugName  = createInfo.debugName,
    });
}

std::unique_ptr<RHIFramebuffer> VulkanRHI::createFramebuffer(const RHIFramebufferCreateInfo& createInfo)
{
    auto framebuffersInfo = VulkanFramebufferInfo({
        .framebufferCount = createInfo.count,
        .renderPass = createInfo.renderPass->as<VulkanRenderPass>()->handle(),
        .extent = { createInfo.extent.width, createInfo.extent.height },
        .device = mDevice.get(),
        .debugName = createInfo.debugName
    });

    for (const auto& attachment : createInfo.attachments)
    {
        vk::ImageView imageView;
        if (std::holds_alternative<RHISwapchain*>(attachment.imageView))
        {
            imageView = std::get<RHISwapchain*>(attachment.imageView)->as<VulkanSwapchain>()->getImageView(attachment.framebufferIndex);
        }

        framebuffersInfo.addAttachment(imageView, attachment.attachmentIndex, attachment.framebufferIndex);
    }

    return VulkanFramebuffer::createVulkanFramebuffer(framebuffersInfo);
}

std::unique_ptr<RHIRenderPass> VulkanRHI::createRenderPass(const RHIRenderPassCreateInfo& createInfo)
{
     auto renderPassInfo = VulkanRenderPassInfo({
        .renderArea = toVulkan(createInfo.renderArea),
        .device     = mDevice.get(),
        .debugName  =  createInfo.debugName,
    });

    for (const auto& colorAttachment : createInfo.colorAttachments)
    {
        renderPassInfo.addColorAttachment(
            toVulkan(colorAttachment.format),
            toVulkan(colorAttachment.finalLayout),
            vk::SampleCountFlagBits::e1,
            colorAttachment.clearValue,
            toVulkan(colorAttachment.loadOp));
    }

    return VulkanRenderPass::createVulkanRenderPass(renderPassInfo);
}

std::unique_ptr<RHIPipeline> VulkanRHI::createPipeline(const RHIPipelineCreateInfo& createInfo)
{
    return VulkanPipeline::createTestPipeline(mDevice.get(), createInfo.renderPass);
}

void VulkanRHI::createInstance()
{
    constexpr auto apiFeatureLevel = VulkanPlatform::getPlatformVulkanFeatureLevel();
    constexpr auto applicationInfo = vk::ApplicationInfo()
        .setApiVersion(apiFeatureLevel)
        .setPApplicationName("VulkanRHI");

    mInstanceLayers = getSupportedInstanceLayers();
    mInstanceExtensions = getSupportedInstanceExtensions(mWindow->getVulkanInstanceExtensions());
    auto instanceCreateInfo = vk::InstanceCreateInfo()
        .setEnabledExtensionCount(mInstanceExtensions.size())
        .setPpEnabledExtensionNames(mInstanceExtensions.data())
        .setEnabledLayerCount(mInstanceLayers.size())
        .setPpEnabledLayerNames(mInstanceLayers.data())
        .setPApplicationInfo(&applicationInfo);

    #ifdef __APPLE__
    instanceCreateInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
    #endif

    try {
        mInstance = vk::createInstance(instanceCreateInfo);
    } catch(const vk::SystemError& error) {
        fmt::println("Failed to create Vulkan Instance: {}", error.what());
        throw;
    }

    VK_VERBOSE("Created Instance");
}

std::vector<const char*> VulkanRHI::getSupportedInstanceLayers(const std::vector<const char*>& additionalLayers)
{
    std::vector<const char*> rhiLayers = {};
    const std::vector driverLayers = vk::enumerateInstanceLayerProperties();

    #ifdef VULKAN_DEBUGGING_ENABLED
    rhiLayers.push_back("VK_LAYER_KHRONOS_validation");
    #endif

    #ifdef VULKAN_API_DUMP
    rhiLayers.push_back("VK_LAYER_LUNARG_api_dump");
    #endif

    if (!additionalLayers.empty())
    {
        std::ranges::copy(additionalLayers, std::inserter(rhiLayers, std::end(rhiLayers)));
    }

    return getSupportedLayers(rhiLayers, driverLayers);
}

std::vector<const char*> VulkanRHI::getSupportedInstanceExtensions(const std::vector<const char*>& additionalExtensions)
{
    std::vector rhiExtensions = {VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
    const std::vector driverExtensions = vk::enumerateInstanceExtensionProperties();

    #ifdef __APPLE__
    rhiExtensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    #endif

    if (!additionalExtensions.empty())
    {
        std::ranges::copy(additionalExtensions, std::inserter(rhiExtensions, std::end(rhiExtensions)));
    }

    return getSupportedExtensions(rhiExtensions, driverExtensions);
}

void VulkanRHI::createDebugContext()
{
    #ifdef VULKAN_DEBUGGING_ENABLED
    mDebugContext = VulkanDebugContext::createVulkanDebugContext({
        .instance = mInstance,
    });
    #endif
}

void VulkanRHI::createDevice()
{
    mDevice = VulkanDevice::createVulkanDevice({
        .instance = mInstance,
    });
}
