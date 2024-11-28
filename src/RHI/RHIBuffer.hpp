#pragma once

#include <cstdint>
#include "Macros.hpp"

class RHIResource
{
public:
    virtual ~RHIResource() = default;
};

class RHIBuffer : public RHIResource
{
public:
    ~RHIBuffer() override = default;

    DEF_AS_CONVERT(RHIBuffer);

    virtual uint64_t getSize()   = 0;
    virtual uint64_t getOffset() = 0;
};