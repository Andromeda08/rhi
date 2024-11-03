#pragma once

#include "VulkanCore.hpp"

using VulkanInstanceExtensions = std::vector<std::shared_ptr<class VulkanInstanceExtension>>;
using VulkanDeviceExtensions   = std::vector<std::shared_ptr<class VulkanDeviceExtension>>;

class VulkanExtension
{
public:
    VulkanExtension(const char* extensionName, const bool requested)
    : mExtensionName(extensionName), mIsRequested(requested)
    {
    }

    virtual ~VulkanExtension() = default;

    const char* extensionName() const { return mExtensionName; }
    bool isEnabled() const { return mIsEnabled; }
    bool isRequested() const { return mIsRequested; }
    virtual bool isSupported() const { return mIsSupported; }

    bool isActive() const { return mIsEnabled && mIsRequested && mIsSupported; }

    bool shouldActivate() const { return mIsRequested && mIsSupported; }

    void setEnabled() { mIsEnabled = true; }
    void setSupported() { mIsSupported = true; }

    static int32_t findExtension(const std::vector<vk::ExtensionProperties>& extensionProperties, const char* layerName);

    std::string toString() const;

protected:
    const char* mExtensionName;
    bool mIsEnabled {false};
    bool mIsRequested {false};
    bool mIsSupported {false};
};

class VulkanInstanceExtension : public VulkanExtension
{
public:
    VulkanInstanceExtension(const char* extensionName, const bool requested)
    : VulkanExtension(extensionName, requested)
    {
    }

    static VulkanInstanceExtensions getRHIInstanceExtensions();
    static std::vector<vk::ExtensionProperties> getDriverInstanceExtensions();
};

class VulkanDeviceExtension : public VulkanExtension
{
public:
    VulkanDeviceExtension(const char* extensionName, const bool requested, uint32_t apiVersion = VK_API_VERSION_1_0)
    : VulkanExtension(extensionName, requested), mApiVersion(apiVersion)
    {
    }

    // e.g. for setting feature options
    virtual void postSupportCheck() {}

    // e.g. for setting pNext
    virtual void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) {}

    static VulkanDeviceExtensions getRHIDeviceExtensions();
    static std::vector<vk::ExtensionProperties> getDriverDeviceExtensions(vk::PhysicalDevice physicalDevice);

private:
    const uint32_t mApiVersion;
};

std::ostream& operator<<(std::ostream& os, const VulkanExtension& vulkanExtension);