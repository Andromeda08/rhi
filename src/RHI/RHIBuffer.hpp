#pragma once

#include <cstdint>
#include "Definitions.hpp"

rhi_BEGIN_NAMESPACE;

struct RHIBufferUploadInfo
{
    const void*       pData          = nullptr;
    uint64_t          dataSize       = 0;
    RHICommandList*   pCommandList   = nullptr;
    RHIBuffer*        pStagingBuffer = nullptr;
};

class RHIBuffer
{
public:
    virtual ~RHIBuffer() = default;

    DEF_AS_CONVERT(RHIBuffer);

    // Set buffer data via memory mapping, only for host-visible buffers.
    virtual void setData(const void* pData, uint64_t dataSize) const = 0;

    // Upload data to a buffer via the specified Staging buffer.
    virtual void uploadData(const RHIBufferUploadInfo& uploadInfo) = 0;

    virtual uint64_t getSize()   = 0;
    virtual uint64_t getOffset() = 0;
};

rhi_END_NAMESPACE;
