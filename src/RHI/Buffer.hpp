#pragma once

#include <cstdint>

class RHIResource
{
public:
    virtual ~RHIResource() = default;
};

class RHIBuffer : public RHIResource
{
public:
    ~RHIBuffer() override = default;

    virtual uint64_t getSize() = 0;
};