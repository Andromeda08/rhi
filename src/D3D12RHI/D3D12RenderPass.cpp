#include "D3D12RenderPass.hpp"

#include "D3D12CommandList.hpp"

D3D12RenderPass::D3D12RenderPass(D3D12RenderPassCreateInfo& createInfo)
: RHIRenderPass(), mRenderTargets(createInfo.perFrameRenderTargets)
{
}

std::unique_ptr<D3D12RenderPass> D3D12RenderPass::createD3D12RenderPass(D3D12RenderPassCreateInfo& createInfo)
{
    return std::make_unique<D3D12RenderPass>(createInfo);
}

void D3D12RenderPass::execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer, std::function<void(RHICommandList*)> lambda)
{
    const auto graphicsCommandList = commandList->as<D3D12CommandList>()->asGraphicsCommandList();

    std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles;
    std::vector<CD3DX12_RESOURCE_BARRIER> rtvBeginBarriers;

    const auto& frameRenderTargets = mRenderTargets[mCounter];

    for (const auto& renderTarget : frameRenderTargets)
    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.rtv, renderTarget.initialState, D3D12_RESOURCE_STATE_RENDER_TARGET);
        rtvBeginBarriers.push_back(barrier);
        rtvHandles.push_back(renderTarget.rtvHandle);
    }

    graphicsCommandList->ResourceBarrier(rtvBeginBarriers.size(), rtvBeginBarriers.data());
    graphicsCommandList->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), false, nullptr);

    for (const auto& renderTarget : frameRenderTargets)
    {
        graphicsCommandList->ClearRenderTargetView(renderTarget.rtvHandle, renderTarget.clearColor.data(), 0, nullptr);
    }

    lambda(commandList);

    std::vector<CD3DX12_RESOURCE_BARRIER> rtvEndBarriers;
    for (const auto& [i, renderTarget] : std::views::enumerate(frameRenderTargets))
    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.rtv, D3D12_RESOURCE_STATE_RENDER_TARGET, renderTarget.initialState);
        rtvEndBarriers.push_back(barrier);
    }
    graphicsCommandList->ResourceBarrier(rtvEndBarriers.size(), rtvEndBarriers.data());

    mCounter = (mCounter + 1) % mRenderTargets.size();
}

std::vector<D3D12RenderTarget> D3D12RenderPass::getRenderTargets(const uint32_t i) const
{
    if (i > mRenderTargets.size())
    {
        throw std::out_of_range(fmt::format("Index {} out of range", i));
    }

    return mRenderTargets[i];
}

