#include "VulkanRHI.hpp"
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
    createInstance();
}

void VulkanRHI::createInstance()
{
    auto appInfo = vk::ApplicationInfo()
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
            rhiExtension->setEnabled();
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

std::shared_ptr<DynamicRHI> VulkanRHI::createVulkanRHI()
{
    return std::make_shared<VulkanRHI>();
}