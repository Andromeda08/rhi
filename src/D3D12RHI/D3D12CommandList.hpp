#pragma once

#include "D3D12Core.hpp"
#include "RHI/Definitions.hpp"
#include "RHI/RHICommandList.hpp"

struct D3D12CommandListParams
{
    ComPtr<ID3D12CommandList>       commandList;
    ComPtr<ID3D12CommandAllocator>  commandAllocator;
    RHICommandQueueType             queueType;
};

class D3D12CommandList : public RHICommandList
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12CommandList, const D3D12CommandListParams& params);

    ~D3D12CommandList() override = default;

    void begin() override;

    void end() override;

    ID3D12CommandList* handle() const { return mCommandList.Get(); }

    ID3D12GraphicsCommandList* asGraphicsCommandList() const;

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance) override;

    void copyBuffer(RHIBuffer* src, RHIBuffer* dst) override {}

    void bindVertexBuffer(RHIBuffer* buffer) override;

    void bindIndexBuffer(RHIBuffer* buffer) override;


private:
    friend class D3D12CommandQueue;

    bool mIsRecording           = false;
    bool mIsGraphicsCommandList = false;

    ComPtr<ID3D12CommandList>      mCommandList;
    ComPtr<ID3D12CommandAllocator> mCommandAllocator;
};