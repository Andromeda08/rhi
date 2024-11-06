#pragma once

#include "Definitions.hpp"

class RHICommandList;

class RHICommandQueue
{
public:
    virtual ~RHICommandQueue() = default;

    virtual RHICommandList*     getCommandList(uint32_t index) = 0;
    virtual RHICommandQueueType getType() = 0;
};