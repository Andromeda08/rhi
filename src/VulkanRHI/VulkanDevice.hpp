#pragma once

#include "VulkanBase.hpp"
#include "VulkanDeviceExtension.hpp"
#include "VulkanCommandQueue.hpp"

struct VulkanDeviceCreateInfo
{
    vk::Instance instance;
};

struct VulkanQueueProperties
{
    vk::QueueFamilyProperties queueFamilyProperties;
    uint32_t                  queueFamilyIndex;
};

class VulkanDevice
{
public:
    DISABLE_COPY_CTOR(VulkanDevice);
    explicit DEF_PRIMARY_CTOR(VulkanDevice, const VulkanDeviceCreateInfo& createInfo);

    ~VulkanDevice() { fmt::println("VulkanDevice DTOR"); }

    void waitIdle() const;

    VulkanCommandQueue*              getGraphicsQueue()  const { return mGraphicsCommandQueue.get(); }

    [[nodiscard]] vk::Device         handle()            const { return mDevice; }
    [[nodisacrd]] vk::PhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }

private:
    void selectPhysicalDevice();

    void createDevice();

    std::optional<VulkanQueueProperties> findQueue(vk::QueueFlags requiredFlags, vk::QueueFlags excludedFlags = {}) const;

    static vk::PhysicalDeviceFeatures getBaseDeviceFeatures();

private:
    vk::Instance                                        mInstance;

    vk::PhysicalDevice                                  mPhysicalDevice;
    vk::PhysicalDeviceProperties                        mPhysicalDeviceProperties;
    std::string                                         mDeviceName;

    vk::Device                                          mDevice;
    std::vector<std::unique_ptr<VulkanDeviceExtension>> mDeviceExtensions;
    std::vector<const char*>                            mDeviceExtensionNames;

    std::unique_ptr<VulkanCommandQueue>                 mGraphicsCommandQueue;
};