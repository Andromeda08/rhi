#include "VulkanRHI.hpp"
#include "VulkanDevice.hpp"

#ifdef VULKAN_DEBUGGING_ENABLED
#include <iostream>
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

VulkanRHI::VulkanRHI()
: DynamicRHI()
{
}

void VulkanRHI::init()
{
    const vk::DynamicLoader dynamicLoader;
    const auto vkGetInstanceProcAddr = dynamicLoader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

    createInstance();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(mInstance);

    createDevice();
    VULKAN_HPP_DEFAULT_DISPATCHER.init(mDevice->handle());

    #ifdef VULKAN_DEBUGGING_ENABLED
    fmt::println("[Info] RHI initialized, using API: {}",
        styled("Vulkan", fg(fmt::color::crimson)));
    #endif
}

void VulkanRHI::createInstance()
{
    constexpr auto appInfo = vk::ApplicationInfo()
        .setPApplicationName("RHI")
        .setApiVersion(VK_API_VERSION_1_3);

    #pragma region "Layer and Extension setup"

    mInstanceLayers = VulkanInstanceLayer::getRHIInstanceLayers();
    const auto driverLayers = VulkanInstanceLayer::getDriverInstanceLayers();
    for (auto& rhiLayer : mInstanceLayers)
    {
        if (VulkanInstanceLayer::findLayer(driverLayers, rhiLayer->layerName()))
        {
            rhiLayer->setEnabled();
        }
    }

    std::vector<const char*> layers;
    for (const auto& layer : mInstanceLayers)
    {
        if (layer->isActive())
        {
            layers.push_back(layer->layerName());
        }
        #if VULKAN_DEBUGGING_ENABLED
        std::cout << layer->toString() << std::endl;
        #endif
    }

    mInstanceExtensions = VulkanInstanceExtension::getRHIInstanceExtensions();
    const auto driverExtensions = VulkanInstanceExtension::getDriverInstanceExtensions();
    for (auto& rhiExtension : mInstanceExtensions)
    {
        if (VulkanInstanceLayer::findLayer(driverLayers, rhiExtension->extensionName()))
        {
            rhiExtension->setSupported();
            if (rhiExtension->isRequested())
            {
                rhiExtension->setEnabled();
            }
        }
    }

    std::vector<const char*> extensions;
    for (const auto& extension : mInstanceExtensions)
    {
        if (extension->isActive())
        {
            extensions.push_back(extension->extensionName());
        }
        #if VULKAN_DEBUGGING_ENABLED
        std::cout << extension->toString() << std::endl;
        #endif
    }

    #pragma endregion

    auto createInfo = vk::InstanceCreateInfo()
        .setEnabledLayerCount(layers.size())
        .setPpEnabledLayerNames(layers.data())
        .setEnabledExtensionCount(extensions.size())
        .setPpEnabledExtensionNames(extensions.data())
        .setPApplicationInfo(&appInfo);

    #ifdef __APPLE__
    createInfo.setFlags(vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR);
    #endif

    if (const auto result = vk::createInstance(&createInfo, nullptr, &mInstance);
        result != vk::Result::eSuccess) {
        throw std::runtime_error("[RHI] Failed to create Vulkan Instance");
    }
}

vk::PhysicalDevice VulkanRHI::selectPhysicalDevice() const
{
    const auto requestedExtensions = VulkanDeviceExtension::getRHIDeviceExtensions();

    const auto physicalDevices = mInstance.enumeratePhysicalDevices();
    const auto candidate = std::ranges::find_if(physicalDevices, [&](const vk::PhysicalDevice& physicalDevice) {
        const vk::PhysicalDeviceProperties props = physicalDevice.getProperties();

        const auto supportedExtensions = VulkanDeviceExtension::getDriverDeviceExtensions(physicalDevice);
        bool requirementsPassed = true;
        for (const auto& extension : requestedExtensions)
        {
            if (!VulkanExtension::findExtension(supportedExtensions, extension->extensionName())
                && extension->isRequested())
            {
                requirementsPassed = false;
            }
        }

        return requirementsPassed;
    });

    if (candidate == std::end(physicalDevices))
    {
        throw std::runtime_error("Failed to find a suitable PhysicalDevice");
    }

    return *candidate;
}

void VulkanRHI::createDevice()
{
    const auto physicalDevice = selectPhysicalDevice();
    mDevice = VulkanDevice::createDevice(physicalDevice);
}

std::shared_ptr<VulkanRHI> VulkanRHI::createVulkanRHI()
{
    return std::make_shared<VulkanRHI>();
}
