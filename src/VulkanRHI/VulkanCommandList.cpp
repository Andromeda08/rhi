#include "VulkanCommandList.hpp"

VulkanCommandList::VulkanCommandList(const vk::CommandBuffer commandBuffer)
: mCommandBuffer(commandBuffer)
{
}

std::shared_ptr<VulkanCommandList> VulkanCommandList::createVulkanCommandList(vk::CommandBuffer commandBuffer)
{
    return std::make_shared<VulkanCommandList>(commandBuffer);
}