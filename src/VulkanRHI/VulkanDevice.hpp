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

    template <typename T>
    void nameObject(const T& handle, const std::string& name, const vk::ObjectType objectType) const
    {
        #ifdef VULKAN_DEBUGGING_ENABLED

        const std::string nameStr = name.empty() ? "Unknown" : name;
        const uint64_t handleU64 = uint64_t(static_cast<typename T::CType>(handle));
        const auto name_info = vk::DebugUtilsObjectNameInfoEXT()
            .setPObjectName(nameStr.c_str())
            .setObjectHandle(handleU64)
            .setObjectType(objectType);

        if (const vk::Result result = mDevice.setDebugUtilsObjectNameEXT(&name_info);
            result != vk::Result::eSuccess)
        {
            fmt::println("[{}] {}",
                styled("VulkanRHI", fg(fmt::color::crimson)),
                fmt::format("Failed to name Vulkan Object {} of type {} as \"{}\"",
                styled(handleU64, fg(fmt::color::light_yellow)),
                styled(to_string(objectType), fg(fmt::color::crimson)),
                name));
        }

        #endif
    }

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