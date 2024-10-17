#include "VulkanLayer.hpp"

int32_t VulkanInstanceLayer::findLayer(const std::vector<vk::LayerProperties>& layerProperties, const char* layerName)
{
    for (int32_t i = 0; i < layerProperties.size(); i++)
    {
        if (!std::strcmp(layerProperties[i].layerName.operator const char*(), layerName))
        {
            return i;
        }
    }
    return -1;
}

VulkanInstanceLayers VulkanInstanceLayer::getRHIInstanceLayers()
{
    VulkanInstanceLayers instanceLayers;

    #define ADD_LAYER(LAYER_NAME, REQUESTED) \
        instanceLayers.push_back(std::make_unique<VulkanInstanceLayer>(LAYER_NAME, REQUESTED));

#if VULKAN_DEBUGGING_ENABLED
    ADD_LAYER("VK_LAYER_KHRONOS_validation", true);
#else
    ADD_LAYER("VK_LAYER_KHRONOS_validation", false);
#endif

    #undef ADD_LAYER

    return instanceLayers;
}

std::vector<vk::LayerProperties> VulkanInstanceLayer::getDriverInstanceLayers()
{
    return vk::enumerateInstanceLayerProperties();
}

