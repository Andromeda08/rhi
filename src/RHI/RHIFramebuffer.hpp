#pragma once

#include <cstdint>
#include "Macros.hpp"

rhi_BEGIN_NAMESPACE;

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

rhi_END_NAMESPACE;
