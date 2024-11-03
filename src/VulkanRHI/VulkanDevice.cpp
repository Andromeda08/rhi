#include "VulkanDevice.hpp"

#include "VulkanCommandQueue.hpp"

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

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    #pragma region "Queues"

    auto queueGraphics = VulkanCommandQueue::findQueue(mPhysicalDevice, RHICommandQueueType::Graphics);

    const float queuePriority = 1.0f;
    std::set uniqueQueueFamilies = { queueGraphics->queueFamilyIndex };
    for (const uint32_t familyIndex : uniqueQueueFamilies)
    {
        const auto queueCreateInfo = vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(familyIndex)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
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
        .setQueueCreateInfoCount(queueCreateInfos.size())
        .setPQueueCreateInfos(queueCreateInfos.data())
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
    VK_PRINTLN(fmt::format("Using PhysicalDevice: {}", styled(mDeviceName, fg(fmt::color::honey_dew))));
    #endif

    #pragma region "Create Queues"

    mGraphicsQueue = VulkanCommandQueue::createVulkanCommandQueue({
        .device = mDevice,
        .type = RHICommandQueueType::Graphics,
        .commandBufferCount = 2,
        .queueProperties = queueGraphics.value(),
    });

    #pragma endregion
}

void VulkanDevice::createSwapchain(const vk::SwapchainCreateInfoKHR& swapchainCreateInfo, vk::SwapchainKHR* pSwapchain) const
{
    if (const vk::Result result = mDevice.createSwapchainKHR(&swapchainCreateInfo, nullptr, pSwapchain);
        result != vk::Result::eSuccess)
    {
        throw std::runtime_error(fmt::format("Failed to create Swapchain ({})", to_string(result)));
    }
}

std::shared_ptr<VulkanDevice> VulkanDevice::createVulkanDevice(vk::PhysicalDevice physicalDevice)
{
    return std::make_shared<VulkanDevice>(physicalDevice);
}
