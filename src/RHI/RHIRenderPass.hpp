#pragma once

#include <functional>

class RHICommandList;
struct RHIFramebufferHandle;

class RHIRenderPass
{
public:
    RHIRenderPass() = default;
    virtual ~RHIRenderPass() = default;

    DEF_AS_CONVERT(RHIRenderPass);

    virtual void execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer, std::function<void(RHICommandList*)> lambda) = 0;
};