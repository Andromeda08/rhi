#include "VulkanCommandQueue.hpp"

VulkanCommandList::VulkanCommandList(const VulkanCommandListCreateInfo& createInfo)
: mCommandList(createInfo.commandBuffer)
, mId(createInfo.id)
{
}

std::unique_ptr<VulkanCommandList> VulkanCommandList::createVulkanCommandList(const VulkanCommandListCreateInfo& createInfo)
{
    return std::make_unique<VulkanCommandList>(createInfo);
}

vk::CommandBuffer VulkanCommandList::getUnderlyingCommandBuffer() const
{
    return mCommandList;
}

VulkanCommandQueue::VulkanCommandQueue(const VulkanCommandQueueCreateInfo& createInfo)
: mDevice(createInfo.device)
{
    VK_EX_CHECK(mQueue = mDevice.getQueue(createInfo.queueFamilyIndex, 0););

    const auto poolCreateInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(createInfo.queueFamilyIndex)
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    VK_EX_CHECK(mCommandPool = mDevice.createCommandPool(poolCreateInfo););

    const auto bufferAllocateInfo = vk::CommandBufferAllocateInfo()
        .setCommandBufferCount(createInfo.commandBufferCount)
        .setCommandPool(mCommandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    uint32_t id = 0;
    for (const std::vector<vk::CommandBuffer> commandBuffers = mDevice.allocateCommandBuffers(bufferAllocateInfo);
         const auto& commandBuffer : commandBuffers)
    {
        mCommandLists.push_back(VulkanCommandList::createVulkanCommandList({
            .commandBuffer = commandBuffer,
            .id            = id,
        }));
        id++;
    }
}

std::unique_ptr<VulkanCommandQueue> VulkanCommandQueue::createVulkanCommandQueue(const VulkanCommandQueueCreateInfo& createInfo)
{
    return std::make_unique<VulkanCommandQueue>(createInfo);
}

VulkanCommandQueue::~VulkanCommandQueue()
{
    std::vector<vk::CommandBuffer> commandBuffers;
    for (const auto& commandList : mCommandLists)
    {
        commandBuffers.push_back(commandList->getUnderlyingCommandBuffer());
    }

    mDevice.freeCommandBuffers(mCommandPool, commandBuffers);
    mCommandLists.clear();

    mDevice.destroyCommandPool(mCommandPool);

    fmt::println("VulkanCommandQueue DTOR");
}

RHICommandList* VulkanCommandQueue::getCommandList(uint32_t id)
{
    if (id >= mCommandLists.size())
    {
        throw std::out_of_range(fmt::format("Index {} out of range for container of size {}",id, mCommandLists.size()));
    }

    return mCommandLists[id].get();
}
