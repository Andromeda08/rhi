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

void D3D12CommandQueue::executeSingleTimeCommand(const std::function<void(RHICommandList*)>& lambda)
{
    ID3D12Fence* fence;
    HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    UINT64 fenceValue = 1;

    D3D12_CHECK(mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)),
        "Failed to create Fence");

    ComPtr<ID3D12CommandAllocator> allocator;
    D3D12_CHECK(mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)),
        "Failed to create CommandAllocator");

    ComPtr<ID3D12CommandList> commandList;
    D3D12_CHECK(mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)),
        "Failed to create CommandList");

    ComPtr<ID3D12GraphicsCommandList> graphicsList;
    D3D12_CHECK(commandList.As(&graphicsList), "Failed to cast to GraphicsCommandList");

    auto rhiList = D3D12CommandList::createD3D12CommandList({
        .commandList = commandList,
        .commandAllocator = allocator,
        .queueType = mType,
    });

    rhiList->mIsRecording = true;

    lambda(rhiList.get());

    graphicsList->Close();
    rhiList->mIsRecording = false;

    ID3D12CommandList* commandLists[] = { commandList.Get() };
    mCommandQueue->ExecuteCommandLists(1, commandLists);
    mCommandQueue->Signal(fence, fenceValue);

    if (fence->GetCompletedValue() < fenceValue)
    {
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    CloseHandle(fenceEvent);
    // commandList->Release();
    // allocator->Release();
    // fence->Release();
}