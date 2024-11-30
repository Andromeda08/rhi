#pragma once

struct RHIFramebufferHandle
{
    RHIFramebufferHandle() = default;
    virtual ~RHIFramebufferHandle() = default;

    DEF_AS_CONVERT(RHIFramebufferHandle);
};

class RHIFramebuffer
{
public:
    RHIFramebuffer() = default;
    virtual ~RHIFramebuffer() = default;

    DEF_AS_CONVERT(RHIFramebuffer);

    virtual RHIFramebufferHandle* getFramebuffer(size_t index) = 0;
};