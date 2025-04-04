#pragma once

#include "VulkanBase.hpp"
#include "RHI/RHICommandList.hpp"
#include "RHI/RHICommandQueue.hpp"

struct VulkanCommandListCreateInfo
{
    vk::CommandBuffer commandBuffer;
    uint32_t          id {};
};

class VulkanCommandList final : public RHICommandList
{
public:
    DISABLE_COPY_CTOR(VulkanCommandList);
    explicit DEF_PRIMARY_CTOR(VulkanCommandList, const VulkanCommandListCreateInfo& createInfo);

    ~VulkanCommandList() override = default;

    void begin() override
    {
        constexpr auto beginInfo = vk::CommandBufferBeginInfo();
        VK_CHECK(mCommandList.begin(beginInfo););
        mIsRecording = true;
    }

    void end() override
    {
        mCommandList.end();
        mIsRecording = false;
    }

    void copyBuffer(RHIBuffer* src, RHIBuffer* dst) override;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override
    {
        mCommandList.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override
    {
        mCommandList.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void bindVertexBuffer(RHIBuffer* buffer) override;

    void bindIndexBuffer(RHIBuffer* buffer) override;

    vk::CommandBuffer handle() const { return mCommandList; }

private:
    friend class VulkanCommandQueue;
    vk::CommandBuffer getUnderlyingCommandBuffer() const;

private:
    vk::CommandBuffer mCommandList;
    uint32_t          mId;

    bool              mIsRecording = false;
};

struct VulkanCommandQueueCreateInfo
{
    vk::Device                device;
    uint32_t                  commandBufferCount;
    vk::QueueFamilyProperties queueFamilyProperties;
    uint32_t                  queueFamilyIndex;
    const char*               debugName;
};

class VulkanCommandQueue final : public RHICommandQueue
{
public:
    DISABLE_COPY_CTOR(VulkanCommandQueue);
    explicit DEF_PRIMARY_CTOR(VulkanCommandQueue, const VulkanCommandQueueCreateInfo& createInfo);

    ~VulkanCommandQueue() override;

    RHICommandList* getCommandList(uint32_t id)                                                  override;
    void            executeSingleTimeCommand(const std::function<void(RHICommandList*)>& lambda) override;

    RHICommandQueueType getType() override { return RHICommandQueueType::Graphics; }

    vk::Queue getQueue() const { return mQueue; }

private:
    vk::Queue                                       mQueue;

    vk::CommandPool                                 mCommandPool;
    std::vector<std::unique_ptr<VulkanCommandList>> mCommandLists;
    std::unique_ptr<VulkanCommandList>              mSingleTimeCommandList;

    vk::Device                                      mDevice;
};
