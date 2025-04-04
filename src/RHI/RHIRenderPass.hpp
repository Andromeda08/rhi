#pragma once

#include <functional>
#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

class RHIRenderPass
{
public:
    RHIRenderPass() = default;
    virtual ~RHIRenderPass() = default;

    DEF_AS_CONVERT(RHIRenderPass);

    virtual void execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer, std::function<void(RHICommandList*)> lambda) = 0;
};

rhi_END_NAMESPACE;
