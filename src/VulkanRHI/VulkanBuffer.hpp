#pragma once

#include "VulkanAllocator.hpp"
#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/Validation.hpp"

struct VulkanBufferCreateInfo
{
    uint64_t            bufferSize;
    RHIBufferType       bufferType;
    VulkanDevice*       pDevice;
    std::string         debugName;
};

class VulkanBuffer : public RHIBuffer
{
public:
    DISABLE_COPY_CTOR(VulkanBuffer);
    explicit DEF_PRIMARY_CTOR(VulkanBuffer, const VulkanBufferCreateInfo& createInfo);

    ~VulkanBuffer() override;

    void setData(const void* pData, const uint64_t dataSize) const override
    {
        void* mappedMemory = mMemory->map();
        std::memcpy(mappedMemory, pData, dataSize);
        mMemory->unmap();
    }

    void uploadData(const RHIBufferUploadInfo& uploadInfo) override;

    vk::DeviceAddress getAddress() const { return mAddress; }
    vk::Buffer        handle()     const { return mBuffer; }

    uint64_t          getSize()    override { return mSize; }
    uint64_t          getOffset()  override { return mMemory->getOffset(); }

private:
    vk::Buffer          mBuffer;
    vk::DeviceSize      mSize;
    vk::DeviceAddress   mAddress {};

    VulkanAllocation*   mMemory;
    VulkanDevice*       mDevice;
};
