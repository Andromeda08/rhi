#include "VulkanDevice.hpp"

#ifdef VULKAN_DEBUGGING_ENABLED
#include <iostream>
#endif

VulkanDevice::VulkanDevice(const vk::PhysicalDevice physicalDevice): mPhysicalDevice(physicalDevice)
{
    mExtensions = VulkanDeviceExtension::getRHIDeviceExtensions();
    std::vector<const char*> supportedExtensionNames;

    #pragma region "Extensions"

    const auto driverExtensions = VulkanDeviceExtension::getDriverDeviceExtensions(mPhysicalDevice);
    for (const auto& extension : mExtensions)
    {
        if (VulkanExtension::findExtension(driverExtensions, extension->extensionName()))
        {
            extension->setSupported();
            if (extension->isRequested())
            {
                extension->setEnabled();
            }
        }
    }

    for (const auto& extension : mExtensions)
    {
        if (extension->shouldActivate())
        {
            if (extension->extensionName() != nullptr)
            {
                supportedExtensionNames.push_back(extension->extensionName());
            }
            extension->postSupportCheck();
        }

        #ifdef VULKAN_DEBUGGING_ENABLED
        std::cout << extension->toString() << std::endl;
        #endif
    }

    #pragma endregion

    auto deviceFeatures = vk::PhysicalDeviceFeatures()
        .setFillModeNonSolid(true)
        .setSamplerAnisotropy(true)
        .setSampleRateShading(true)
        .setShaderInt64(true);

    auto deviceCreateInfo = vk::DeviceCreateInfo()
        .setEnabledExtensionCount(supportedExtensionNames.size())
        .setPpEnabledExtensionNames(supportedExtensionNames.data())
        .setQueueCreateInfoCount(0)
        .setPQueueCreateInfos(nullptr)
        .setPEnabledFeatures(&deviceFeatures);

    for (const auto& extensions : mExtensions)
    {
        extensions->preCreateDevice(deviceCreateInfo);
    }

    if (const auto result = mPhysicalDevice.createDevice(&deviceCreateInfo, nullptr, &mDevice);
        result != vk::Result::eSuccess)
    {
        throw std::runtime_error(fmt::format("Failed to create Device ({})", to_string(result)));
    }

    mPhysicalDeviceProps = mPhysicalDevice.getProperties();
    mDeviceName = mPhysicalDeviceProps.deviceName.data();

    #ifdef VULKAN_DEBUGGING_ENABLED
    fmt::println("[Info] Using PhysicalDevice: {}",
        styled(mDeviceName, fg(fmt::color::honey_dew)));
    #endif
}
