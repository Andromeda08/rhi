#pragma once

#include "VulkanCore.hpp"
#include "RHI/CommandList.hpp"

class VulkanCommandList : public RHICommandList
{
public:
    explicit DEF_PRIMARY_CTOR(VulkanCommandList, vk::CommandBuffer commandBuffer);

    void begin() override {}
    void end() override {}

    ~VulkanCommandList() override = default;

private:
    vk::CommandBuffer mCommandBuffer;
};