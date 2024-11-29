#pragma once

#include "VulkanBase.hpp"

class VulkanDeviceExtension
{
public:
    explicit DEF_PRIMARY_CTOR(VulkanDeviceExtension, const char* extensionName, const bool requested);
    virtual ~VulkanDeviceExtension() = default;

    // e.g. for setting feature options
    virtual void postSupportCheck() {}

    // e.g. for setting pNext
    virtual void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) {}

    const char* extensionName()  const { return mExtensionName; }
    bool        isRequested()    const { return mIsRequested; }
    bool        isSupported()    const { return mIsSupported; }
    bool        isActive()       const { return mIsRequested and mIsSupported and mIsEnabled; }
    bool        shouldActivate() const;

    void setSupported() { mIsSupported = true; }
    void setEnabled()   { mIsEnabled = true; }

    static std::vector<std::unique_ptr<VulkanDeviceExtension>> getRHIDeviceExtensions();
    static std::vector<vk::ExtensionProperties>                getDriverDeviceExtensions(vk::PhysicalDevice physicalDevice);

    // Returns the list of RHI supported Device Extensions with device support evaluation.
    static std::vector<std::unique_ptr<VulkanDeviceExtension>> getEvaluatedRHIDeviceExtensions(vk::PhysicalDevice physicalDevice);

protected:
    const char* mExtensionName  = nullptr;
    bool        mIsRequested    = false;
    bool        mIsSupported    = false;
    bool        mIsEnabled      = false;
};
