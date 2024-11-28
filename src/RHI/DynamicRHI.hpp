#pragma once

#include <memory>
#include "Buffer.hpp"
#include "Definitions.hpp"
#include "Frame.hpp"
#include "Framebuffer.hpp"
#include "IPipeline.hpp"
#include "RenderPass.hpp"

class  RHICommandQueue;
class  RHISwapchain;
struct Frame;

struct RHIFrameBeginInfo
{
    bool useSwapchain = true;
};

class DynamicRHI
{
public:
    virtual ~DynamicRHI() = default;

    /**
     * RHI Commands
     */
    virtual void waitIdle() const = 0;

    virtual Frame beginFrame(const RHIFrameBeginInfo& frameBeginInfo) = 0;
    virtual void  submitFrame(const Frame& frame) = 0;

    virtual std::unique_ptr<RHIBuffer> createBuffer(const RHIBufferCreateInfo& createInfo) = 0;

    virtual std::unique_ptr<RHIFramebuffers> createFramebuffers(RHIRenderPass* renderPass) = 0;

    virtual std::unique_ptr<RHIRenderPass> createRenderPass() = 0;

    virtual std::unique_ptr<IPipeline> createTestPipeline(RHIRenderPass* renderPass) = 0;

    // virtual std::shared_ptr<class RHITexture>  createTexture(const struct RHICreateTextureParams& params) = 0;
    // virtual std::shared_ptr<class RHIPipeline> createPipeline(const struct RHICreatePipelineParams& params) = 0;

    virtual RHICommandQueue* getGraphicsQueue() = 0;

    virtual RHISwapchain* getSwapchain() const = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }
};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;