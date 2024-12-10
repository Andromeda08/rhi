#pragma once

#include "D3D12CommandList.hpp"
#include "D3D12Core.hpp"
#include "RHI/RHICommandQueue.hpp"

class D3D12CommandList;

struct D3D12CommandQueueParams
{
    ID3D12Device*        device;
    RHICommandQueueType  type = RHICommandQueueType::Graphics;
    uint32_t             commandListCount = 2;
};

class D3D12CommandQueue : public RHICommandQueue
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12CommandQueue, const D3D12CommandQueueParams& params);

    RHICommandList* getCommandList(uint32_t index) override
    {
        if (index > mCommandAllocators.size())
        {
            throw std::out_of_range(fmt::format("Index {} is out of bounds for container of size {}", index, mCommandAllocators.size()));
        }
        return mCommandLists[index].get();
    }

    RHICommandQueueType getType() override { return mType; }

    static D3D12_COMMAND_LIST_TYPE getD3D12QueueType(RHICommandQueueType queueType);

    ComPtr<ID3D12CommandQueue> getQueueHandle() const { return mCommandQueue; }

    void executeSingleTimeCommand(const std::function<void(RHICommandList*)>& lambda) override {}

private:
    RHICommandQueueType        mType;
    ComPtr<ID3D12CommandQueue> mCommandQueue;

    // 1 CommandAllocator per requested CommandList (1-1 map)
    std::vector<ComPtr<ID3D12CommandAllocator>> mCommandAllocators;
    std::vector<ComPtr<ID3D12CommandList>>      mRawCommandLists;

    std::vector<std::unique_ptr<D3D12CommandList>> mCommandLists;

    ID3D12Device* mDevice;
};