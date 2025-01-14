#pragma once

#include <cstdint>
#include <fmt/format.h>
#include "Macros.hpp"
#include "fmt/color.h"

class RHIBuffer;
class RHICommandList;

struct RHIBufferUploadInfo
{
    const void*       pData;
    uint64_t          dataSize {0};
    RHICommandList*   pCommandList;
    RHIBuffer*        pStagingBuffer;
};

class RHIBuffer
{
public:
    virtual ~RHIBuffer() = default;

    DEF_AS_CONVERT(RHIBuffer);

    virtual void setData(const void* pData, uint64_t dataSize) const = 0;

    /**
     * Upload data to a buffer via a Staging buffer.
     */
    virtual void uploadData(const RHIBufferUploadInfo& uploadInfo) = 0;

    virtual uint64_t getSize()   = 0;
    virtual uint64_t getOffset() = 0;
};