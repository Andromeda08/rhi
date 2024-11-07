#pragma once

#include <functional>
#include "Definitions.hpp"

class RHICommandList;

class RHICommandQueue
{
public:
    virtual ~RHICommandQueue() = default;

    virtual RHICommandList*     getCommandList(uint32_t index) = 0;
    virtual RHICommandQueueType getType() = 0;

    virtual void executeSingleTimeCommand(const std::function<void(RHICommandList*)>& lambda) = 0;
};