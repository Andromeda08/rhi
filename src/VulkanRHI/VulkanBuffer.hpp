#pragma once

#include "VulkanBase.hpp"
#include "RHI/Buffer.hpp"

class VulkanBuffer : public RHIBuffer
{
public:
    DISABLE_COPY_CTOR(VulkanBuffer);
    explicit DEF_PRIMARY_CTOR(VulkanBuffer, const RHIBufferCreateInfo& createInfo);

    ~VulkanBuffer() override = default;

private:
    vk::Buffer mBuffer;
};