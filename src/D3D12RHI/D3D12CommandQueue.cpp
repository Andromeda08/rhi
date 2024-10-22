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
}

std::shared_ptr<D3D12CommandQueue> D3D12CommandQueue::createD3D12CommandQueue(const D3D12CommandQueueParams& params)
{
    return std::make_shared<D3D12CommandQueue>(params);
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
