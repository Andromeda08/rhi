#pragma once

struct RHIFramebuffer
{
    RHIFramebuffer() = default;
    virtual ~RHIFramebuffer() = default;

    DEF_AS_CONVERT(RHIFramebuffer);
};

class RHIFramebuffers
{
public:
    RHIFramebuffers() = default;
    virtual ~RHIFramebuffers() = default;

    virtual RHIFramebuffer* getFramebuffer(size_t index) = 0;

    DEF_AS_CONVERT(RHIFramebuffers);
};