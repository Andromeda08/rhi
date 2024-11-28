#pragma once

class RHICommandList;

class IPipeline
{
public:
    IPipeline() = default;
    virtual ~IPipeline() = default;

    virtual void bind(RHICommandList* commandList) = 0;
};