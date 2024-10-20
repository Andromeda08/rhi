#pragma once

#include "VulkanCore.hpp"

using VulkanInstanceLayers = std::vector<std::unique_ptr<class VulkanInstanceLayer>>;

class VulkanInstanceLayer
{
public:
    VulkanInstanceLayer(const char* layerName, bool requested)
    : mLayerName(layerName), mIsRequested(requested)
    {
    }

    const char* layerName() const { return mLayerName; }
    bool isEnabled() const { return mIsEnabled; }
    bool isRequested() const { return mIsRequested; }

    bool isActive() const { return mIsEnabled && mIsRequested; }

    void setEnabled() { mIsEnabled = true; }

    static int32_t findLayer(const std::vector<vk::LayerProperties>& layerProperties, const char* layerName);
    static VulkanInstanceLayers getRHIInstanceLayers();
    static std::vector<vk::LayerProperties> getDriverInstanceLayers();

    std::string toString() const;

private:
    const char* mLayerName;
    bool mIsEnabled {false};
    bool mIsRequested {false};
};
