#pragma once

#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

class RHIPipeline
{
public:
    RHIPipeline() = default;
    virtual ~RHIPipeline() = default;

    virtual void bind(RHICommandList* commandList) = 0;
};

rhi_END_NAMESPACE;
