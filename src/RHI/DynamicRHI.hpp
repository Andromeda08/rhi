#pragma once

#include <memory>
#include "RHIDefinitions.hpp"
#include "IWindow.hpp"

class RHICommandQueue;

class DynamicRHI
{
public:
    virtual ~DynamicRHI() = default;

    /**
     * RHI Commands
     */
    virtual void waitIdle() const = 0;
    // virtual void syncNextFrame() = 0;
    // virtual std::shared_ptr<class RHIBuffer>   createBuffer(const struct RHICreateBufferParams& params) = 0;
    // virtual std::shared_ptr<class RHITexture>  createTexture(const struct RHICreateTextureParams& params) = 0;
    // virtual std::shared_ptr<class RHIPipeline> createPipeline(const struct RHICreatePipelineParams& params) = 0;

    virtual RHICommandQueue* getGraphicsQueue() = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }
};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;