#include "VulkanBuffer.hpp"

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& createInfo)
: RHIBuffer()
, mDevice(createInfo.pDevice)
{
    const auto baseUsageFlags = vk::BufferUsageFlags();
    const auto bufferCreateInfo = vk::BufferCreateInfo()
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSize(createInfo.bufferSize)
        .setUsage(baseUsageFlags);

    VK_CHECK(mBuffer = mDevice->handle().createBuffer(bufferCreateInfo););

    const auto basePropertyFlags = vk::MemoryPropertyFlags();
    const auto allocationInfo = VulkanAllocationInfo()
        .setTarget(mBuffer)
        .setPropertyFlags(basePropertyFlags);

    mMemory  = mDevice->allocateMemory(allocationInfo);
    mAddress = mMemory->getAddress().value();

    mDevice->nameObject<vk::Buffer>({
        .debugName  = createInfo.debugName,
        .handle     = mBuffer,
    });

    VK_VERBOSE(fmt::format("Created Buffer ({})", createInfo.debugName));
}
