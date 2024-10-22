#pragma once

class RHICommandList
{
public:
    virtual ~RHICommandList() = default;

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
    // virtual void draw() = 0;

    /**
     * Transfer operations
     */
    // virtual void copyBuffer() = 0;
    // virtual void copyImage() = 0;

    /**
     * Synchronization
     */
    // virtual void synchronize() = 0;

protected:
    bool mIsRecording = false;
};
