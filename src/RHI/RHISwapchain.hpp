#pragma once

#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

class RHISwapchain
{
public:
    RHISwapchain() = default;
    virtual ~RHISwapchain() = default;

    DEF_AS_CONVERT(RHISwapchain);

    virtual uint32_t getNextFrameIndex(uint32_t currentFrame) const = 0;

    virtual void setScissorViewport(RHICommandList* commandList) const = 0;

    virtual Rect2D getScissor() = 0;
    virtual Viewport getViewport() = 0;

    virtual Size2D getSize() const = 0;
    virtual float getAspectRatio() const = 0;
    virtual Format getFormat() const = 0;
    virtual uint32_t getFrameCount() = 0;
};

rhi_END_NAMESPACE;
