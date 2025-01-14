#pragma once

#include <memory>
#include "RHIBuffer.hpp"
#include "Definitions.hpp"
#include "Frame.hpp"
#include "RHIFramebuffer.hpp"
#include "RHIPipeline.hpp"
#include "RHIRenderPass.hpp"
#include "RHITexture.hpp"

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
    virtual void waitIdle() = 0;

    virtual Frame beginFrame(const RHIFrameBeginInfo& frameBeginInfo) = 0;
    virtual void  submitFrame(const Frame& frame) = 0;

    virtual std::unique_ptr<RHIBuffer> createBuffer(const RHIBufferCreateInfo& createInfo) = 0;

    virtual std::unique_ptr<RHITexture> createTexture(const RHITextureCreateInfo& createInfo) = 0;

    virtual std::unique_ptr<RHIFramebuffer> createFramebuffer(const RHIFramebufferCreateInfo& createInfo) = 0;

    virtual std::unique_ptr<RHIRenderPass> createRenderPass(const RHIRenderPassCreateInfo& createInfo) = 0;

    virtual std::unique_ptr<RHIPipeline> createPipeline(const RHIPipelineCreateInfo& createInfo) = 0;

    // virtual std::shared_ptr<class RHITexture>  createTexture(const struct RHICreateTextureParams& params) = 0;
    // virtual std::shared_ptr<class RHIPipeline> createPipeline(const struct RHICreatePipelineParams& params) = 0;

    virtual RHICommandQueue* getGraphicsQueue() = 0;

    virtual RHISwapchain* getSwapchain() const = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }
};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;