#pragma once

#include "RHIDefinitions.hpp"

class RHICommandList;

class RHICommandQueue
{
public:
    virtual ~RHICommandQueue() = default;

    virtual std::shared_ptr<RHICommandList> getCommandList(uint32_t index) = 0;

    // virtual std::shared_ptr<RHICommandList> getSingleTimeCommandList(uint32_t index) = 0;

    virtual RHICommandQueueType getType() = 0;
};