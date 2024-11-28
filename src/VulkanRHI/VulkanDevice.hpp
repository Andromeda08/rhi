#pragma once

#include "VulkanAllocator.hpp"
#include "VulkanBase.hpp"
#include "VulkanCommandQueue.hpp"
#include "VulkanDeviceExtension.hpp"

struct VulkanDeviceCreateInfo
{
    vk::Instance instance;
};

struct VulkanQueueProperties
{
    vk::QueueFamilyProperties queueFamilyProperties;
    uint32_t                  queueFamilyIndex;
};

struct VulkanAllocationInfo
{
    vk::MemoryPropertyFlags             propertyFlags;
    std::variant<vk::Buffer, vk::Image> target;

    auto& setPropertyFlags(const vk::MemoryPropertyFlags value)
    {
        propertyFlags = value;
        return *this;
    }

    template <class T>
    auto& setTarget(const T& handle)
    {
        target = handle;
        return *this;
    }
};

template <class T>
struct VulkanNameObjectInfo
{
    const char*    debugName;
    T              handle;
};

class VulkanDevice
{
public:
    DISABLE_COPY_CTOR(VulkanDevice);
    explicit DEF_PRIMARY_CTOR(VulkanDevice, const VulkanDeviceCreateInfo& createInfo);

    ~VulkanDevice() { waitIdle(); }

    void waitIdle() const;

    // Returns a non-owning pointer to the allocated memory
    VulkanAllocation*                allocateMemory(const VulkanAllocationInfo& allocationInfo);

    template <class T>
    void                             nameObject(const VulkanNameObjectInfo<T>& nameObjectInfo) const;

    VulkanCommandQueue*              getGraphicsQueue()  const { return mGraphicsCommandQueue.get(); }

    [[nodiscard]] vk::Device         handle()            const { return mDevice; }
    [[nodisacrd]] vk::PhysicalDevice getPhysicalDevice() const { return mPhysicalDevice; }

private:
    void selectPhysicalDevice();

    void createDevice();

    std::optional<VulkanQueueProperties> findQueue(vk::QueueFlags requiredFlags, vk::QueueFlags excludedFlags = {}) const;

    uint32_t findMemoryHeapIndex(uint32_t filter, vk::MemoryPropertyFlags propertyFlags) const;

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

    std::vector<std::unique_ptr<VulkanAllocation>>      mMemoryAllocations;
};

template<class T>
void VulkanDevice::nameObject(const VulkanNameObjectInfo<T>& nameObjectInfo) const
{
    #ifdef VULKAN_DEBUGGING_ENABLED
    const std::string name = nameObjectInfo.debugName ? nameObjectInfo.debugName : "Unknown";
    const auto nameInfo = vk::DebugUtilsObjectNameInfoEXT()
        .setPObjectName(name.c_str())
        .setObjectHandle(uint64_t(static_cast<T::CType>(nameObjectInfo.handle)))
        .setObjectType(nameObjectInfo.handle.objectType);

    VK_CHECK(mDevice.setDebugUtilsObjectNameEXT(nameInfo););
    #endif
}
