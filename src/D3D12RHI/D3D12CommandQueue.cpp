#include "D3D12CommandQueue.hpp"

D3D12CommandQueue::D3D12CommandQueue(const D3D12CommandQueueParams& params)
: mType(params.type)
, mDevice(params.device)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type  = getD3D12QueueType(mType);
    D3D12_CHECK(mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)),
        "Failed to create CommandQueue.");

    mCommandAllocators.resize(params.commandListCount);
    mRawCommandLists.resize(params.commandListCount);

    for (uint32_t i = 0; i < params.commandListCount; i++)
    {
        D3D12_CHECK(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[i])),
            "Failed to create CommandAllocator");

        D3D12_CHECK(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[i].Get(), nullptr, IID_PPV_ARGS(&mRawCommandLists[i])),
            "Failed to create CommandList");

        if (mType == RHICommandQueueType::Graphics)
        {
            ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
            D3D12_CHECK(mRawCommandLists[i].As(&graphicsCommandList), "Failed to cast to GraphicsCommandList");
            D3D12_CHECK(graphicsCommandList->Close(), "Failed to close GraphicsCommandList");
        }

        mCommandLists.push_back(D3D12CommandList::createD3D12CommandList({
            .commandList = mRawCommandLists[i],
            .commandAllocator = mCommandAllocators[i],
            .queueType = mType,
        }));
    }
}

std::unique_ptr<D3D12CommandQueue> D3D12CommandQueue::createD3D12CommandQueue(const D3D12CommandQueueParams& params)
{
    return std::make_unique<D3D12CommandQueue>(params);
}

D3D12_COMMAND_LIST_TYPE D3D12CommandQueue::getD3D12QueueType(RHICommandQueueType queueType)
{
    switch (queueType)
    {
        case RHICommandQueueType::Graphics:
            return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
        case RHICommandQueueType::AsyncCompute:
            return D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_COMPUTE;
    }
    throw std::runtime_error("Unsupported RHICommandQueueType");
}
