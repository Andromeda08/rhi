#pragma once

#include "VulkanCore.hpp"
#include "VulkanExtension.hpp"

class VulkanCommandQueue;

class VulkanDevice
{
public:
    explicit VulkanDevice(vk::PhysicalDevice physicalDevice);

    std::shared_ptr<VulkanCommandQueue> getGraphicsQueue() const { return mGraphicsQueue; }

    void createSwapchain(const vk::SwapchainCreateInfoKHR& swapchainCreateInfo, vk::SwapchainKHR* pSwapchain) const;

    vk::PhysicalDevice physicalDevice() const { return mPhysicalDevice; }
    vk::Device handle() const { return mDevice; }
    const std::string& getPhysicalDeviceName() const { return mDeviceName; }

    static std::shared_ptr<VulkanDevice> createDevice(vk::PhysicalDevice physicalDevice)
    {
        return std::make_shared<VulkanDevice>(physicalDevice);
    }

private:
    std::shared_ptr<VulkanCommandQueue> mGraphicsQueue;

    vk::Device mDevice;
    VulkanDeviceExtensions mExtensions;

    vk::PhysicalDevice mPhysicalDevice;
    std::string mDeviceName;
    vk::PhysicalDeviceProperties mPhysicalDeviceProps;
};