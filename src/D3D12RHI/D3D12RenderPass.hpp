#pragma once

#include "D3D12Core.hpp"
#include "RHI/RHICommandList.hpp"
#include "RHI/RHIRenderPass.hpp"

struct D3D12RenderTarget
{
    DXGI_FORMAT                     format;
    ID3D12Resource*                 rtv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE   rtvHandle;
    std::array<float, 4>            clearColor { 0.0f, 0.0f, 0.0f, 1.0f };
    D3D12_RESOURCE_STATES           initialState;
};

struct D3D12DepthTarget
{
    DXGI_FORMAT                   format;
    ID3D12Resource*               dsv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle;
    float                         depthClear;
    uint32_t                      stencilClear;
    D3D12_RESOURCE_STATES         initialState;
};

struct D3D12RenderPassCreateInfo
{
    std::vector<std::vector<D3D12RenderTarget>> perFrameRenderTargets;
    D3D12DepthTarget depthTarget;
};

/**
 * D3D12 has no concept of render passes, but is required due to Vulkan's needs.
 * Instead, this class carries the information required to set up render targets when rendering,
 * handled in the `D3D12RenderPass::execute()` method.
 */
class D3D12RenderPass final : public RHIRenderPass
{
public:
    DISABLE_COPY_CTOR(D3D12RenderPass);
    DEF_PRIMARY_CTOR(D3D12RenderPass, D3D12RenderPassCreateInfo& createInfo);

    ~D3D12RenderPass() override = default;

    void execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer, std::function<void(RHICommandList*)> lambda) override;

    std::vector<D3D12RenderTarget> getRenderTargets(const uint32_t i) const;

    std::optional<D3D12DepthTarget> getDSV() const { return mDepthTarget; }

private:
    std::vector<std::vector<D3D12RenderTarget>> mRenderTargets;
    std::optional<D3D12DepthTarget>             mDepthTarget { std::nullopt };
    uint32_t mCounter = 0; // TODO: 💀
};
