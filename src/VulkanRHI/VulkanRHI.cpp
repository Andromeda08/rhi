#include "VulkanRHI.hpp"

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

    VK_PRINTLN(fmt::format("{} RHI initialized", VK_STYLED_PREFIX));
}

std::unique_ptr<VulkanRHI> VulkanRHI::createVulkanRHI(const VulkanRHICreateInfo& createInfo)
{
    return std::make_unique<VulkanRHI>(createInfo);
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
