#pragma once

#include "VulkanAllocator.hpp"
#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/Buffer.hpp"

struct VulkanBufferCreateInfo
{
    uint64_t            bufferSize;
    RHIBufferType       bufferType;
    VulkanDevice*       pDevice;
    const char*         debugName;
};

class VulkanBuffer : public RHIBuffer
{
public:
    DISABLE_COPY_CTOR(VulkanBuffer);
    explicit DEF_PRIMARY_CTOR(VulkanBuffer, const VulkanBufferCreateInfo& createInfo);

    ~VulkanBuffer() override = default;

    uint64_t getSize() override { return mSize; }

private:
    vk::Buffer          mBuffer;
    vk::DeviceSize      mSize;
    vk::DeviceAddress   mAddress {};

    VulkanAllocation*   mMemory;
    VulkanDevice*       mDevice;
};

inline std::unique_ptr<VulkanBuffer>
VulkanBuffer::createVulkanBuffer(const VulkanBufferCreateInfo& createInfo)
{
    return std::make_unique<VulkanBuffer>(createInfo);
}