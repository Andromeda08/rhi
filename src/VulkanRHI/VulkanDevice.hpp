#pragma once

#include "VulkanCore.hpp"
#include "VulkanExtension.hpp"

class VulkanDevice
{
public:
    DISABLE_COPY_CTOR(VulkanDevice);
    explicit DEF_PRIMARY_CTOR(VulkanDevice, vk::PhysicalDevice physicalDevice);


    void createSwapchain(const vk::SwapchainCreateInfoKHR& swapchainCreateInfo, vk::SwapchainKHR* pSwapchain) const;

    template <typename T>
    void nameObject(const T& handle, const std::string& name, vk::ObjectType objectType) const;


    std::shared_ptr<VulkanCommandQueue> getGraphicsQueue() const { return mGraphicsQueue; }

    vk::PhysicalDevice  physicalDevice()        const { return mPhysicalDevice; }
    vk::Device          handle()                const { return mDevice; }
    const std::string&  getPhysicalDeviceName() const { return mDeviceName; }

private:
    std::shared_ptr<VulkanCommandQueue> mGraphicsQueue;

    vk::Device                          mDevice;
    VulkanDeviceExtensions              mExtensions;

    vk::PhysicalDevice                  mPhysicalDevice;
    vk::PhysicalDeviceProperties        mPhysicalDeviceProps;
    std::string                         mDeviceName;
};

template<typename T>
void VulkanDevice::nameObject(const T& handle, const std::string& name, const vk::ObjectType objectType) const
{
#ifdef VULKAN_DEBUGGING_ENABLED

    const std::string nameStr = name.empty() ? "Unknown" : name;
    const auto handleU64 = uint64_t(static_cast<typename T::CType>(handle));
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
