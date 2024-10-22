#include "VulkanCommandQueue.hpp"

#include "VulkanCommandList.hpp"

VulkanCommandQueue::VulkanCommandQueue(const VulkanCommandQueueParams& params)
: RHICommandQueue()
, mDevice(params.device)
, mType(params.type)
{
    const auto poolCreateInfo = vk::CommandPoolCreateInfo()
        .setQueueFamilyIndex(params.queueProperties.queueFamilyIndex)
        .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

    mCommandPool = mDevice.createCommandPool(poolCreateInfo);

    const auto bufferAllocateInfo = vk::CommandBufferAllocateInfo()
        .setCommandBufferCount(params.commandBufferCount)
        .setCommandPool(mCommandPool)
        .setLevel(vk::CommandBufferLevel::ePrimary);

    mRawCommandLists.resize(params.commandBufferCount);
    auto result = mDevice.allocateCommandBuffers(&bufferAllocateInfo, mRawCommandLists.data());

    for (const auto& commandBuffer : mRawCommandLists)
    {
        mCommandLists.push_back(VulkanCommandList::createVulkanCommandList(commandBuffer));
    }
}

std::shared_ptr<VulkanCommandQueue> VulkanCommandQueue::createVulkanCommandQueue(const VulkanCommandQueueParams& params)
{
    return std::make_shared<VulkanCommandQueue>(params);
}

vk::QueueFlags VulkanCommandQueue::getRequiredQueueFlags(const RHICommandQueueType queueType)
{
    switch (queueType)
    {
        case RHICommandQueueType::Graphics:
            return vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute;
        case RHICommandQueueType::AsyncCompute:
            return vk::QueueFlagBits::eCompute;
    }
    throw std::runtime_error("Unsupported RHICommandQueueType");
}

vk::QueueFlags VulkanCommandQueue::getExcludedQueueFlags(const RHICommandQueueType queueType)
{
    switch (queueType)
    {
        case RHICommandQueueType::Graphics:
            return {};
        case RHICommandQueueType::AsyncCompute:
            return vk::QueueFlagBits::eGraphics;
    }
    throw std::runtime_error("Unsupported RHICommandQueueType");
}

auto VulkanCommandQueue::findQueue(const vk::PhysicalDevice physicalDevice, const RHICommandQueueType queueType)
-> std::optional<QueueProperties>
{
    return findQueue(physicalDevice, getRequiredQueueFlags(queueType), getExcludedQueueFlags(queueType));
}

auto VulkanCommandQueue::findQueue(const vk::PhysicalDevice physicalDevice, const vk::QueueFlags requiredFlags, const vk::QueueFlags excludedFlags)
-> std::optional<QueueProperties>
{
    const std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    const auto it = std::ranges::find_if(queueFamilies,[requiredFlags, excludedFlags](const vk::QueueFamilyProperties& properties){
        return (properties.queueCount > 0)
           && (properties.queueFlags & requiredFlags)
           && !(properties.queueFlags & excludedFlags);
    });

    if (it == std::end(queueFamilies))
    {
        return std::nullopt;
    }

    const uint32_t familyIndex = static_cast<uint32_t>((it - std::begin(queueFamilies)));
    QueueProperties queueProperties = { *it, familyIndex };
    return std::make_optional(queueProperties);
}
