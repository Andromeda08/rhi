#pragma once

#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

struct RHITextureCreateInfo
{
    Size2D      size        = {};
    Format      format      = Format::R32G32B32A32Sfloat;
    bool        sampled     = true;
    std::string debugName   = {};
};

class RHITexture
{
public:
    virtual ~RHITexture() = default;

    DEF_AS_CONVERT(RHITexture);
};

rhi_END_NAMESPACE;
