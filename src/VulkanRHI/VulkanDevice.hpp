#pragma once

#include <vulkan/vulkan.hpp>
#include "Core/Base.hpp"
#include "VulkanExtension.hpp"

class VulkanDevice
{
public:
    explicit VulkanDevice(vk::PhysicalDevice physicalDevice);

    vk::PhysicalDevice physicalDevice() const { return mPhysicalDevice; }
    vk::Device handle() const { return mDevice; }
    const std::string& getPhysicalDeviceName() const { return mDeviceName; }

    static std::shared_ptr<VulkanDevice> createDevice(vk::PhysicalDevice physicalDevice)
    {
        return std::make_shared<VulkanDevice>(physicalDevice);
    }

private:
    vk::PhysicalDevice mPhysicalDevice;

    vk::Device mDevice;
    VulkanDeviceExtensions mExtensions;

    std::string mDeviceName;
    vk::PhysicalDeviceProperties mPhysicalDeviceProps;
};