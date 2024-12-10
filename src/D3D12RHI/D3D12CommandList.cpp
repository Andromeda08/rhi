#include "D3D12CommandList.hpp"

D3D12CommandList::D3D12CommandList(const D3D12CommandListParams& params)
: RHICommandList()
, mCommandList(params.commandList)
, mCommandAllocator(params.commandAllocator)
{
    /**
     * TODO: Quite a naive check, this is not necessarily true,
     * "Graphics" is doing alot of heavy lifting here...
     */
    if (params.queueType == RHICommandQueueType::Graphics)
    {
        mIsGraphicsCommandList = true;
    }
}

std::unique_ptr<D3D12CommandList> D3D12CommandList::createD3D12CommandList(const D3D12CommandListParams& params)
{
    return std::make_unique<D3D12CommandList>(params);
}

void D3D12CommandList::begin()
{
    if (mIsRecording)
    {
        fmt::println("{}", styled("CommandList is already recording", fg(fmt::color::crimson)));
        throw std::exception();
    }

    D3D12_CHECK(mCommandAllocator->Reset(), "Failed to reset CommandAllocator");

    if (mIsGraphicsCommandList)
    {
        const auto graphicsCommandList = asGraphicsCommandList();
        D3D12_CHECK(graphicsCommandList->Reset(mCommandAllocator.Get(), nullptr),
                    "Failed to reset GraphicsCommandList");
    }

    mIsRecording = true;
}

void D3D12CommandList::end()
{
    if (!mIsRecording)
    {
        fmt::println("{}", styled("CommandList is not in a recording state", fg(fmt::color::crimson)));
        throw std::exception();
    }

    if (mIsGraphicsCommandList)
    {
        const auto graphicsCommandList = asGraphicsCommandList();
        D3D12_CHECK(graphicsCommandList->Close(), "Failed to close GraphicsCommandList");
    }

    mIsRecording = false;
}

ID3D12GraphicsCommandList* D3D12CommandList::asGraphicsCommandList() const
{
    if (!mIsGraphicsCommandList)
    {
        return nullptr;
    }

    ComPtr<ID3D12GraphicsCommandList> graphicsCommandList;
    D3D12_CHECK(mCommandList.As(&graphicsCommandList), "Couldn't cast to GraphicsCommandList");

    return graphicsCommandList.Get();
}

void D3D12CommandList::draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    if (!mIsGraphicsCommandList)
    {
        fmt::println("{} called on a non-graphics CommandList", styled("D3D12CommandList::draw()", fg(fmt::color::light_yellow)));
        return;
    }

    const auto graphicsCommandList = asGraphicsCommandList();
    graphicsCommandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    graphicsCommandList->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
}
