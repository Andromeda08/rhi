#include "VulkanExtension.hpp"

int32_t VulkanInstanceExtension::findExtension(const std::vector<vk::LayerProperties>& extensionProperties, const char* extensionName)
{
    for (int32_t i = 0; i < extensionProperties.size(); i++)
    {
        if (!std::strcmp(extensionProperties[i].layerName.operator const char*(), extensionName))
        {
            return i;
        }
    }
    return -1;
}

VulkanInstanceExtensions VulkanInstanceExtension::getRHIInstanceExtensions()
{
    VulkanInstanceExtensions instanceExtensions;

    #define ADD_EXTENSION(EXT_NAME, REQUESTED) \
        instanceExtensions.push_back(std::make_unique<VulkanInstanceExtension>(EXT_NAME, REQUESTED));

    #if __APPLE__
        ADD_EXTENSION(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, true);
    #else
        ADD_EXTENSION(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, false);
    #endif

    #undef ADD_EXTENSION

    return instanceExtensions;
}

std::vector<vk::ExtensionProperties> VulkanInstanceExtension::getDriverInstanceExtensions()
{
    return vk::enumerateInstanceExtensionProperties();
}
