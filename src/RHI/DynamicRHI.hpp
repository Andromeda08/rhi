#pragma once

#include <memory>
#include "RHIDefinitions.hpp"
#include "IWindow.hpp"

class RHICommandQueue;

class DynamicRHI
{
public:
    virtual ~DynamicRHI() = default;

    virtual void init(const std::shared_ptr<IRHIWindow>& window) = 0;

    virtual std::shared_ptr<RHICommandQueue> getGraphicsQueue() = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }

    // RHI Commands
    // virtual void waitIdle() = 0;
    // virtual void syncNextFrame() = 0;
};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;