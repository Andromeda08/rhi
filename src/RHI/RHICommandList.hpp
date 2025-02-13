#pragma once

#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

class RHICommandList
{
public:
    RHICommandList() = default;
    virtual ~RHICommandList() = default;

    DEF_AS_CONVERT(RHICommandList);

    // Begin recording of CommandList
    virtual void begin() = 0;
    // End recording of CommandList
    virtual void end() = 0;

    /**
     * Rendering related commands
     */
    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;
    virtual void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) = 0;
    virtual void bindVertexBuffer(RHIBuffer* buffer) = 0;
    virtual void bindIndexBuffer(RHIBuffer* buffer) = 0;

    /**
     * Transfer operations
     */
    virtual void copyBuffer(RHIBuffer* src, RHIBuffer* dst) = 0;

protected:
    bool mIsRecording = false;
};

rhi_END_NAMESPACE;
