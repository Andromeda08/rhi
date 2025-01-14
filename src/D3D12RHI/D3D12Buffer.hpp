#pragma once

#include "D3D12Core.hpp"
#include "RHI/RHIBuffer.hpp"

struct D3D12BufferCreateInfo
{
    uint64_t        bufferSize;
    RHIBufferType   bufferType;
    D3D12Device*    pDevice;
    const wchar_t*  debugName;
};

class D3D12Buffer : public RHIBuffer
{
public:
    DISABLE_COPY_CTOR(D3D12Buffer);
    explicit DEF_PRIMARY_CTOR(D3D12Buffer, const D3D12BufferCreateInfo& createInfo);

    ~D3D12Buffer() override;

    void setData(const void* pData, uint64_t dataSize) const override;

    void uploadData(const RHIBufferUploadInfo& uploadInfo) override;

    uint64_t getSize() override { return mAllocation->GetSize(); }

    uint64_t getOffset() override { return mAllocation->GetOffset(); }

    D3D12_VERTEX_BUFFER_VIEW& getVertexBufferView();

    D3D12_INDEX_BUFFER_VIEW& getIndexBufferView();

private:
    uint64_t                    mSize;
    D3D12MA::Allocation*        mAllocation;
    ID3D12Resource*             mResource;

    D3D12_GPU_VIRTUAL_ADDRESS   mAddress;
    D3D12_RESOURCE_STATES       mState { D3D12_RESOURCE_STATE_COMMON };
    D3D12_HEAP_TYPE             mHeapType;

    D3D12_VERTEX_BUFFER_VIEW    mVertexBufferView {};
    D3D12_INDEX_BUFFER_VIEW     mIndexBufferView {};

    RHIBufferType               mBufferType;
    const wchar_t*              mDebugName {};
    D3D12Device*                mDevice;
};