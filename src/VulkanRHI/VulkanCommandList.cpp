#include "VulkanCommandList.hpp"

VulkanCommandList::VulkanCommandList(const vk::CommandBuffer commandBuffer)
: mCommandBuffer(commandBuffer)
{
}

std::shared_ptr<VulkanCommandList> VulkanCommandList::createVulkanCommandList(vk::CommandBuffer commandBuffer)
{
    return std::make_shared<VulkanCommandList>(commandBuffer);
}

void VulkanCommandList::begin()
{
    if (mIsRecording)
    {
        throw std::runtime_error("CommandList is already in recording state");
    }
    mIsRecording = true;

    constexpr auto beginInfo = vk::CommandBufferBeginInfo();
    mCommandBuffer.begin(beginInfo);
}

void VulkanCommandList::end()
{
    if (mIsRecording)
    {
        throw std::runtime_error("Cannot end recording of a CommandList that is not in recording state.");
    }
    mCommandBuffer.end();
    mIsRecording = false;
}
