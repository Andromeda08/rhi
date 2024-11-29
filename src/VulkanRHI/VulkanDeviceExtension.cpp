#include "VulkanDeviceExtension.hpp"

#pragma region "Vulkan Device Extensions"

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
        if (shouldActivate())
        {
            addToPNext(deviceCreateInfo, mVulkanCore11);
        }
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
        if (shouldActivate())
        {
            addToPNext(deviceCreateInfo, mVulkanCore12);
        }
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
        if (shouldActivate())
        {
            addToPNext(deviceCreateInfo, mVulkanCore13);
        }
    }

private:
    vk::PhysicalDeviceVulkan13Features mVulkanCore13;
};

class VulkanSwapchainExtension final : public VulkanDeviceExtension
{
public:
    explicit VulkanSwapchainExtension() : VulkanDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME, true) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override {}
};

class VulkanSynchronization2Extension final : public VulkanDeviceExtension
{
public:
    explicit VulkanSynchronization2Extension() : VulkanDeviceExtension(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, true) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
        mSynchronization2 = vk::PhysicalDeviceSynchronization2Features()
            .setSynchronization2(true);
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override
    {
        if (shouldActivate())
        {
            addToPNext(deviceCreateInfo, mSynchronization2);
        }
    }

private:
    vk::PhysicalDeviceSynchronization2Features mSynchronization2;
};

class VulkanPortabilitySubsetExtension final : public VulkanDeviceExtension
{
public:
    explicit VulkanPortabilitySubsetExtension(const bool required = false)
    : VulkanDeviceExtension("VK_KHR_portability_subset", required) {}

    void postSupportCheck() override
    {
        if (!shouldActivate()) return;
        mIsEnabled = true;
    }

    void preCreateDevice(vk::DeviceCreateInfo& deviceCreateInfo) override {}

    // For now this method assumes that only Apple devices require this...
    static std::unique_ptr<VulkanPortabilitySubsetExtension> makePlatformSpecific()
    {
        bool required = false;
        #ifdef __APPLE__
            required = true;
        #endif
        return std::make_unique<VulkanPortabilitySubsetExtension>(required);
    }
};

#pragma endregion

VulkanDeviceExtension::VulkanDeviceExtension(const char* extensionName, const bool requested)
: mExtensionName(extensionName), mIsRequested(requested)
{
}

std::unique_ptr<VulkanDeviceExtension> VulkanDeviceExtension::createVulkanDeviceExtension(const char* extensionName,const bool requested)
{
    return std::make_unique<VulkanDeviceExtension>(extensionName, requested);
}

std::vector<std::unique_ptr<VulkanDeviceExtension>> VulkanDeviceExtension::getEvaluatedRHIDeviceExtensions(const vk::PhysicalDevice physicalDevice)
{
    auto rhiExtensions = getRHIDeviceExtensions();
    const auto driverExtensions = getDriverDeviceExtensions(physicalDevice);

    for (const auto& extension : rhiExtensions)
    {
        if (findExtension(extension->extensionName(), driverExtensions))
        {
            extension->setSupported();
            if (extension->isRequested())
            {
                extension->setEnabled();
            }
        }
    }

    return rhiExtensions;
}

bool VulkanDeviceExtension::shouldActivate() const
{
    return mIsRequested and mIsSupported;
}

std::vector<std::unique_ptr<VulkanDeviceExtension>> VulkanDeviceExtension::getRHIDeviceExtensions()
{
    std::vector<std::unique_ptr<VulkanDeviceExtension>> deviceExtensions;

    #define ADD_CORE(TYPE)  deviceExtensions.push_back(std::make_unique<TYPE>());
    #define ADD_BASIC(TYPE) deviceExtensions.push_back(std::make_unique<TYPE>());

    ADD_CORE(VulkanCore11);
    ADD_CORE(VulkanCore12);

    #ifndef __APPLE__
    ADD_CORE(VulkanCore13);
    #else
    ADD_BASIC(VulkanSynchronization2Extension);
    #endif

    ADD_BASIC(VulkanSwapchainExtension);

    deviceExtensions.push_back(VulkanPortabilitySubsetExtension::makePlatformSpecific());

    #undef ADD_BASIC
    #undef ADD_CORE

    return deviceExtensions;
}

std::vector<vk::ExtensionProperties> VulkanDeviceExtension::getDriverDeviceExtensions(const vk::PhysicalDevice physicalDevice)
{
    return  physicalDevice.enumerateDeviceExtensionProperties();
}
