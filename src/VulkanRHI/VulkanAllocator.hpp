#pragma once

#include "VulkanBase.hpp"

struct VulkanAllocationCreateInfo
{
    vk::Device                          device;
    vk::DeviceMemory                    memory;
    vk::DeviceSize                      size {};
    std::variant<vk::Buffer, vk::Image> target;
};

class VulkanAllocation
{
public:
    DISABLE_COPY_CTOR(VulkanAllocation);
    DEF_PRIMARY_CTOR(VulkanAllocation, const VulkanAllocationCreateInfo& createInfo);

    ~VulkanAllocation();

    void  bind();
    void* map();
    void  unmap();
    void  free();

    vk::DeviceMemory getMemory() const { return mMemory; };

    // Contains a value if the allocation was created for a Buffer
    std::optional<vk::DeviceAddress> getAddress() const { return mAddress; }

private:
           bool                                mInvalid = false;

           vk::DeviceMemory                    mMemory;
           std::optional<vk::DeviceAddress>    mAddress {std::nullopt};
    const  vk::DeviceSize                      mOffset {0};
    const  vk::DeviceSize                      mSize;

    const  std::variant<vk::Buffer, vk::Image> mUser;
    const  vk::Device                          mDevice;

    const  uint32_t                            mId;
    static uint32_t                            sSequence;
};