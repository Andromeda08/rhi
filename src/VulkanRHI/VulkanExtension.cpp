#include "VulkanExtension.hpp"
#include "VulkanUtil.hpp"

int32_t VulkanExtension::findExtension(const std::vector<vk::ExtensionProperties>& extensionProperties, const char* extensionName)
{
    for (int32_t i = 0; i < extensionProperties.size(); i++)
    {
        if (!std::strcmp(extensionProperties[i].extensionName.operator const char*(), extensionName))
        {
            return i;
        }
    }
    return -1;
}

std::string VulkanExtension::toString() const
{
    return fmt::format("[Device] {} (Active={}, Requested={}, Supported={})",
        styled(mExtensionName, fg(fmt::color::cornflower_blue)),
        isActive() ? "y" : "n",
        isRequested() ? "y" : "n",
        isSupported() ? "y" : "n");
}

#pragma region "Instance"

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

#pragma endregion

#pragma region "Device"

std::vector<vk::ExtensionProperties> VulkanDeviceExtension::getDriverDeviceExtensions(vk::PhysicalDevice physicalDevice)
{
    return physicalDevice.enumerateDeviceExtensionProperties();
}

class VulkanCore11 final : public VulkanDeviceExtension
{
public:
    explicit VulkanCore11(): VulkanDeviceExtension({}, true) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
        mVulkanCore11 = vk::PhysicalDeviceVulkan11Features();
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkanCore11);
    }

private:
    vk::PhysicalDeviceVulkan11Features mVulkanCore11;
};

class VulkanCore12 final : public VulkanDeviceExtension
{
public:
    explicit VulkanCore12(): VulkanDeviceExtension({}, true) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
        mVulkanCore12 = vk::PhysicalDeviceVulkan12Features()
            .setBufferDeviceAddress(true)
            .setDescriptorIndexing(true)
            .setScalarBlockLayout(true)
            .setShaderInt8(true)
            .setTimelineSemaphore(true)
            .setHostQueryReset(true)
            .setScalarBlockLayout(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkanCore12);
    }

private:
    vk::PhysicalDeviceVulkan12Features mVulkanCore12;
};

class VulkanCore13 final : public VulkanDeviceExtension
{
public:
    explicit VulkanCore13(): VulkanDeviceExtension({}, true) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
        mVulkanCore13 = vk::PhysicalDeviceVulkan13Features()
            .setMaintenance4(true)
            .setDynamicRendering(true)
            .setSynchronization2(true)
            .setInlineUniformBlock(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        addToPNext(deviceCreateInfo, mVulkanCore13);
    }

private:
    vk::PhysicalDeviceVulkan13Features mVulkanCore13;
};

VulkanDeviceExtensions VulkanDeviceExtension::getRHIDeviceExtensions()
{
    VulkanDeviceExtensions deviceExtensions;

    #define ADD_CORE(TYPE) \
        deviceExtensions.push_back(std::make_unique<TYPE>());

    ADD_CORE(VulkanCore11);
    ADD_CORE(VulkanCore12);
    ADD_CORE(VulkanCore13);

    #undef ADD_CORE

    return deviceExtensions;
}

#pragma endregion
