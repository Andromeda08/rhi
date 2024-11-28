#pragma once

#include "Macros.hpp"

class RHICommandList;

class RHIPipeline
{
public:
    RHIPipeline() = default;
    virtual ~RHIPipeline() = default;

    virtual void bind(RHICommandList* commandList) = 0;
};