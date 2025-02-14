#pragma once

#include <functional>
#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

class RHICommandQueue
{
public:
    RHICommandQueue() = default;
    virtual ~RHICommandQueue() = default;

    DEF_AS_CONVERT(RHICommandQueue);

    virtual RHICommandList*     getCommandList(uint32_t index) = 0;
    virtual RHICommandQueueType getType() = 0;

    virtual void executeSingleTimeCommand(const std::function<void(RHICommandList*)>& lambda) = 0;
};

rhi_END_NAMESPACE;
