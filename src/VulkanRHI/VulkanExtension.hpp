#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/Base.hpp"

using VulkanInstanceExtensions = std::vector<std::unique_ptr<class VulkanInstanceExtension>>;
using VulkanDeviceExtensions   = std::vector<std::unique_ptr<class VulkanDeviceExtension>>;

class VulkanExtension
{
public:
    VulkanExtension(const char* extensionName, bool requested)
    : mLayerName(extensionName), mIsRequested(requested)
    {
    }

    const char* layerName() const { return mLayerName; }
    bool isEnabled() const { return mIsEnabled; }
    bool isRequested() const { return mIsRequested; }

    bool isActive() const { return mIsEnabled && mIsRequested; }

    void setEnabled() { mIsEnabled = true; }

    static int32_t findExtension(const std::vector<vk::LayerProperties>& layerProperties, const char* layerName);

protected:
    const char* mLayerName;
    bool mIsEnabled {false};
    bool mIsRequested {false};
};

class VulkanInstanceExtension : public VulkanExtension
{
public:
    VulkanInstanceExtension(const char* extensionName, bool requested)
    : VulkanExtension(extensionName, requested)
    {
    }

    static int32_t findExtension(const std::vector<vk::LayerProperties>& extensionProperties, const char* extensionName);
    static VulkanInstanceExtensions getRHIInstanceExtensions();
    static std::vector<vk::ExtensionProperties> getDriverInstanceExtensions();
};