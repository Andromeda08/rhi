#pragma once

#include "VulkanBase.hpp"
#include "RHI/CommandList.hpp"
#include "RHI/CommandQueue.hpp"

struct VulkanCommandListCreateInfo
{
    vk::CommandBuffer commandBuffer;
    uint32_t          id {};
};

class VulkanCommandList : public RHICommandList
{
public:
    DISABLE_COPY_CTOR(VulkanCommandList);
    explicit DEF_PRIMARY_CTOR(VulkanCommandList, const VulkanCommandListCreateInfo& createInfo);

    ~VulkanCommandList() override = default;

    void begin() override {}

    void end() override {}

private:
    friend class VulkanCommandQueue;
    vk::CommandBuffer getUnderlyingCommandBuffer() const;

    vk::CommandBuffer mCommandList;
    uint32_t          mId;
};

struct VulkanCommandQueueCreateInfo
{
    vk::Device                device;
    uint32_t                  commandBufferCount;
    vk::QueueFamilyProperties queueFamilyProperties;
    uint32_t                  queueFamilyIndex;
    const char*               debugName;
};

class VulkanCommandQueue : public RHICommandQueue
{
public:
    DISABLE_COPY_CTOR(VulkanCommandQueue);
    explicit DEF_PRIMARY_CTOR(VulkanCommandQueue, const VulkanCommandQueueCreateInfo& createInfo);

    ~VulkanCommandQueue() override;

    [[nodiscard]] RHICommandList* getCommandList(uint32_t id) override;

    RHICommandQueueType getType() override { return RHICommandQueueType::Graphics; }

private:
    vk::Queue                                       mQueue;

    vk::CommandPool                                 mCommandPool;
    std::vector<std::unique_ptr<VulkanCommandList>> mCommandLists;

    vk::Device                                      mDevice;
};
