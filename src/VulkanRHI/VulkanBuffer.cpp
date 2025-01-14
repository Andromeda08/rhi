#include "VulkanBuffer.hpp"

struct BufferTypeFlags
{
    vk::MemoryPropertyFlags memoryFlags;
    vk::BufferUsageFlags    usageFlags;

    static BufferTypeFlags forType(RHIBufferType bufferType);
};

BufferTypeFlags BufferTypeFlags::forType(const RHIBufferType bufferType)
{
    BufferTypeFlags result = {
        .memoryFlags = {},
        .usageFlags = vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc,
    };

    switch (bufferType)
    {
        using enum RHIBufferType;
        using enum vk::BufferUsageFlagBits;
        using enum vk::MemoryPropertyFlagBits;

        case Index: {
            result.usageFlags  |= eIndexBuffer | eStorageBuffer | eAccelerationStructureBuildInputReadOnlyKHR;
            result.memoryFlags |= eDeviceLocal;
            break;
        }
        case Staging: {
            result.usageFlags  |= eTransferSrc;
            result.memoryFlags |= eHostVisible | eHostCoherent;
            break;
        }
        case Storage: {
            result.usageFlags  |= eStorageBuffer;
            result.memoryFlags |= eHostVisible | eHostCoherent;
            break;
        }
        case Uniform: {
            result.usageFlags  |= eUniformBuffer;
            result.memoryFlags |= eHostVisible | eHostCoherent;
            break;
        }
        case Vertex: {
            result.usageFlags  |= eVertexBuffer | eStorageBuffer | eAccelerationStructureBuildInputReadOnlyKHR;
            result.memoryFlags |= eDeviceLocal;
            break;
        }
        default:
            break;
    }
    return result;
}

VulkanBuffer::VulkanBuffer(const VulkanBufferCreateInfo& createInfo)
: RHIBuffer()
, mDevice(createInfo.pDevice)
{
    const auto [memoryFlags, usageFlags] = BufferTypeFlags::forType(createInfo.bufferType);
    const auto bufferCreateInfo = vk::BufferCreateInfo()
        .setSharingMode(vk::SharingMode::eExclusive)
        .setSize(createInfo.bufferSize)
        .setUsage(usageFlags);

    VK_CHECK(mBuffer = mDevice->handle().createBuffer(bufferCreateInfo););

    const auto allocationInfo = VulkanAllocationInfo()
        .setTarget(mBuffer)
        .setPropertyFlags(memoryFlags);

    mMemory  = mDevice->allocateMemory(allocationInfo);
    mMemory->bind();

    mAddress = mMemory->getAddress().value();
    mSize    = createInfo.bufferSize;

    mDevice->nameObject<vk::Buffer>({
        .debugName  = createInfo.debugName.c_str(),
        .handle     = mBuffer,
    });

    VK_VERBOSE(fmt::format("Created Buffer (debugName: {})", createInfo.debugName.empty() ? "-" : createInfo.debugName));
}

std::unique_ptr<VulkanBuffer> VulkanBuffer::createVulkanBuffer(const VulkanBufferCreateInfo& createInfo)
{
    return std::make_unique<VulkanBuffer>(createInfo);
}

VulkanBuffer::~VulkanBuffer()
{
    mDevice->handle().destroyBuffer(mBuffer);
    mMemory->free();
}

void VulkanBuffer::uploadData(const RHIBufferUploadInfo& uploadInfo)
{
    VulkanBuffer* stagingBuffer = uploadInfo.pStagingBuffer->as<VulkanBuffer>();

    stagingBuffer->setData(uploadInfo.pData);

    const auto bufferCopy = vk::BufferCopy()
        .setSize(uploadInfo.dataSize)
        .setSrcOffset(0)
        .setDstOffset(0);

    auto* commandBuffer = uploadInfo.pCommandList->as<VulkanCommandList>();
    commandBuffer->handle().copyBuffer(stagingBuffer->mBuffer, mBuffer, 1, &bufferCopy);
}
