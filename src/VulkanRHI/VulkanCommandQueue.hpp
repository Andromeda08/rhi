#pragma once

#include "VulkanCore.hpp"
#include "RHI/CommandQueue.hpp"

struct QueueProperties
{
    vk::QueueFamilyProperties queueFamilyProperties;
    uint32_t queueFamilyIndex;
};

struct VulkanCommandQueueParams
{
    vk::Device          device;
    RHICommandQueueType type = RHICommandQueueType::Graphics;
    uint32_t            commandBufferCount = 2;
    QueueProperties     queueProperties;
};

class VulkanCommandQueue : public RHICommandQueue
{
public:
    explicit DEF_PRIMARY_CTOR(VulkanCommandQueue, const VulkanCommandQueueParams&);

    std::shared_ptr<RHICommandList> getCommandList(uint32_t index) override { return nullptr; }

    RHICommandQueueType getType() override { return mType; }

    ~VulkanCommandQueue() override = default;

    /**
     * Queue utility methods
     */
    static vk::QueueFlags getRequiredQueueFlags(RHICommandQueueType queueType);
    static vk::QueueFlags getExcludedQueueFlags(RHICommandQueueType queueType);
    static std::optional<QueueProperties> findQueue(vk::PhysicalDevice physicalDevice, RHICommandQueueType queueType);
    static std::optional<QueueProperties> findQueue(vk::PhysicalDevice physicalDevice, vk::QueueFlags requiredFlags, vk::QueueFlags excludedFlags);

private:
    vk::Queue mQueue;
    vk::CommandPool mCommandPool;
    std::vector<std::shared_ptr<VulkanCommandList>> mCommandLists;
    std::vector<vk::CommandBuffer> mRawCommandLists;

    vk::Device mDevice;
    RHICommandQueueType mType;
};
