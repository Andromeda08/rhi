#pragma once

#include <memory>
#include "RHIDefinitions.hpp"

class DynamicRHI
{
public:
    virtual ~DynamicRHI() = default;

    virtual void init() = 0;

    virtual RHIInterfaceType getType() const { return RHIInterfaceType::None; }

    // RHI Commands

};

// Global pointer to the current RHI implementation
extern std::shared_ptr<DynamicRHI> gRHI;