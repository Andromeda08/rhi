#pragma once

class RHIBuffer;

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
    // virtual void beginRenderPass() = 0;
    // virtual void endRenderPass() = 0;
    // virtual void bindPipeline() = 0;
    // virtual void bindDescriptorSet() = 0;
    // virtual void pushConstants() = 0;
    virtual void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) = 0;

    /**
     * Transfer operations
     */
    virtual void copyBuffer(RHIBuffer* src, RHIBuffer* dst) = 0;
    // virtual void copyImage() = 0;

    /**
     * Synchronization
     */
    // virtual void synchronize() = 0;

protected:
    bool mIsRecording = false;
};
