#pragma once

#include <functional>

class RHICommandList;
struct RHIFramebuffer;

class RHIRenderPass
{
public:
    RHIRenderPass() = default;
    virtual ~RHIRenderPass() = default;

    virtual void execute(RHICommandList* commandList, RHIFramebuffer* framebuffer, std::function<void(RHICommandList*)> lambda) = 0;

    DEF_AS_CONVERT(RHIRenderPass);
};