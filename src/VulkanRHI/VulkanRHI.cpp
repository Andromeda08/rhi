#include "VulkanRHI.hpp"

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
        auto sci = vk::SemaphoreCreateInfo();
        auto fci = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

        result = mDevice->handle().createSemaphore(&sci, nullptr, &mImageReady[i]);
        result = mDevice->handle().createSemaphore(&sci, nullptr, &mRenderingFinished[i]);
        result = mDevice->handle().createFence(&fci, nullptr, &mFrameInFlight[i]);
    }

    VK_PRINTLN(fmt::format("{} RHI initialized", VK_STYLED_PREFIX));
}

std::unique_ptr<VulkanRHI> VulkanRHI::createVulkanRHI(const VulkanRHICreateInfo& createInfo)
{
    return std::make_unique<VulkanRHI>(createInfo);
}

Frame VulkanRHI::beginFrame(const RHIFrameBeginInfo& frameBeginInfo)
{
    // auto acquireNextImageInfo = vk::AcquireNextImageInfoKHR()
    //     .setFence(mFrameInFlight[mCurrentFrame])
    //     .setSemaphore(mImageReady[mCurrentFrame])
    //     .setSwapchain(mSwapchain->handle())
    //     .setTimeout(std::numeric_limits<uint64_t>::max());
    //
    // uint32_t nextImage;
    // try {
    //     nextImage = mDevice->handle().acquireNextImage2KHR(acquireNextImageInfo).value;
    // }
    // catch (const vk::OutOfDateKHRError& err)
    // {
    //     const auto msg = "Swapchain Out of Date and it should be recreated";
    //     VK_PRINTLN(styled(msg, fg(fmt::color::yellow)));
    //     throw std::runtime_error(msg);
    // }
    // catch (const vk::SystemError& err)
    // {
    //     const auto msg = fmt::format("Failed to acquire next Swapchain image: {}", err.what());
    //     VK_PRINTLN(styled(msg, fg(fmt::color::yellow)));
    //     throw std::runtime_error(msg);
    // }

    vk::Result result;
    vk::Fence fence = mFrameInFlight[mCurrentFrame];
    result = mDevice->handle().waitForFences(1, &fence, true, std::numeric_limits<uint64_t>::max());
    result = mDevice->handle().resetFences(1, &fence);
    auto nextImage = mDevice->handle().acquireNextImageKHR(mSwapchain->handle(), std::numeric_limits<uint64_t>::max(),
                                                  mImageReady[mCurrentFrame], nullptr).value;

    return {
        .mCurrentFrame = mCurrentFrame,
        .mAcquiredFrameIndex = nextImage,
    };
}

void VulkanRHI::submitFrame(const Frame& frame)
{
    std::vector<vk::CommandBufferSubmitInfo> commandBufferSubmitInfos;
    for (const auto commandBuffer : frame.mCommandLists)
    {
        const auto info = vk::CommandBufferSubmitInfo()
            .setCommandBuffer(commandBuffer->as<VulkanCommandList>()->handle());
        commandBufferSubmitInfos.push_back(info);
    }

    const auto frameIndex = frame.getCurrentFrame();

    const auto waitSemaphoreInfo = vk::SemaphoreSubmitInfo()
        .setSemaphore(mImageReady[frameIndex])
        .setStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput);
    std::vector waitSemaphoreInfos = { waitSemaphoreInfo };

    const auto signalSemaphoreInfo = vk::SemaphoreSubmitInfo()
        .setSemaphore(mImageReady[frameIndex])
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
        throw std::runtime_error("big bad");
    }

    mSwapchain->present(mImageReady[frameIndex], frame.getAcquiredFrameIndex());

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

std::unique_ptr<RHIFramebuffers> VulkanRHI::createFramebuffers(const RHIFramebuffersCreateInfo& createInfo)
{
    auto framebuffersInfo = VulkanFramebuffersInfo({
        .framebufferCount = 2,
        .renderPass = createInfo.renderPass->as<VulkanRenderPass>()->handle(),
        .extent = mSwapchain->getExtent(),
        .device = mDevice.get(),
        .debugName = "Test Framebuffer"
    })
    .addAttachment(mSwapchain->getImageView(0), 0, 0)
    .addAttachment(mSwapchain->getImageView(1), 0, 1);

    return VulkanFramebuffers::createVulkanFramebuffers(framebuffersInfo);
}

std::unique_ptr<RHIRenderPass> VulkanRHI::createRenderPass(const RHIRenderPassCreateInfo& createInfo)
{
    const auto renderPassInfo = VulkanRenderPassInfo({
        .renderArea = {{0, 0}, mSwapchain->getExtent()},
        .device     = mDevice.get(),
        .debugName  = "Test RenderPass",
    })
    .addColorAttachment(mSwapchain->getFormatVk(), vk::ImageLayout::ePresentSrcKHR);
    return VulkanRenderPass::createVulkanRenderPass(renderPassInfo);
}

std::unique_ptr<RHIPipeline> VulkanRHI::createPipeline(const RHIPipelineCreateInfo& createInfo)
{
    return VulkanPipeline::createTestPipeline(mDevice.get(), createInfo.renderPass);
}

void VulkanRHI::createInstance()
{
    constexpr auto applicationInfo = vk::ApplicationInfo()
        .setApiVersion(VK_API_VERSION_1_3)
        .setPApplicationName("Vulkan RHI");

    mInstanceLayers = getSupportedInstanceLayers();
    mInstanceExtensions = getSupportedInstanceExtensions(mWindow->getVulkanInstanceExtensions());
    const auto instanceCreateInfo = vk::InstanceCreateInfo()
        .setEnabledExtensionCount(mInstanceExtensions.size())
        .setPpEnabledExtensionNames(mInstanceExtensions.data())
        .setEnabledLayerCount(mInstanceLayers.size())
        .setPpEnabledLayerNames(mInstanceLayers.data())
        .setPApplicationInfo(&applicationInfo);

    #ifdef __APPLE__
    createInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
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

    #ifdef __APPLE_
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
