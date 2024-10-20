#pragma once

#include <memory>
#include "RHIDefinitions.hpp"

class DynamicRHI
{
public:
    virtual ~DynamicRHI() = default;

    virtual void init() = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }

    uint32_t currentFrameIndex() const { return mFrameIndex; }

    // RHI Commands
    // virtual void waitIdle() = 0;

    // virtual void syncNextFrame() = 0;

protected:
    uint32_t mFrameIndex {0};
};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;