#pragma once

#include "D3D12Core.hpp"
#include "RHI/RHIDefinitions.hpp"
#include "RHI/CommandList.hpp"

struct D3D12CommandListParams
{
    ComPtr<ID3D12CommandList> commandList;
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    RHICommandQueueType queueType;
};

class D3D12CommandList : public RHICommandList
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12CommandList, const D3D12CommandListParams& params);

    void begin() override {}
    void end() override {}

private:
    bool mIsRecording           = false;
    bool mIsGraphicsCommandList = false;

    ComPtr<ID3D12CommandList>      mCommandList;
    ComPtr<ID3D12CommandAllocator> mCommandAllocator;
};