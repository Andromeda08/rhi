#pragma once

#include "VulkanAllocator.hpp"
#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/RHIBuffer.hpp"

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

    template <class T>
    static std::unique_ptr<VulkanBuffer> createVulkanBufferWithData(const VulkanBufferCreateInfo& createInfo, T* pData, RHICommandList* pCommandList)
    {
        auto buffer = createVulkanBuffer(createInfo);
        const auto stagingBuffer = createVulkanBuffer({
            .bufferSize = createInfo.bufferSize,
            .bufferType = Staging,
            .pDevice    = createInfo.pDevice,
        });
        stagingBuffer->setData(pData);

        pCommandList->copyBuffer(buffer.get(), stagingBuffer.get());

        return buffer;
    }

    ~VulkanBuffer() override;

    void setData(const void* pData) const
    {
        void* mappedMemory = mMemory->map();
        std::memcpy(mappedMemory, pData, mMemory->getSize());
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
