#pragma once

#include "D3D12CommandList.hpp"
#include "D3D12Core.hpp"
#include "D3D12Device.hpp"
#include "D3D12RenderPass.hpp"
#include "RHI/RHICommandList.hpp"
#include "RHI/RHIPipeline.hpp"

struct D3D12GraphicsPipelineStateInfo
{
    CD3DX12_RASTERIZER_DESC    rasterizerState   = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    CD3DX12_BLEND_DESC         blendState        = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    CD3DX12_DEPTH_STENCIL_DESC depthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    D3D12GraphicsPipelineStateInfo& setCullMode(const D3D12_CULL_MODE cullMode)
    {
        rasterizerState.CullMode = cullMode;
        return *this;
    }
};

struct D3D12PipelineCreateInfo
{
    std::vector<D3D12_INPUT_ELEMENT_DESC>   inputElements {};

    std::vector<RHIShaderCreateInfo>        shadersCreateInfos {};
    D3D12RenderPass*                        renderPass = nullptr;
    PipelineType                            pipelineType { PipelineType::Graphics };
    D3D12GraphicsPipelineStateInfo          graphicsPiplineState {};

    D3D12Device*                            device = nullptr;
    const char*                             debugName;
};

class D3D12Pipeline : public RHIPipeline
{
public:
    DISABLE_COPY_CTOR(D3D12Pipeline);
    explicit DEF_PRIMARY_CTOR(D3D12Pipeline, D3D12PipelineCreateInfo& createInfo);

    ~D3D12Pipeline() override;

    void bind(RHICommandList* commandList) override
    {
        // Insight: CommandLists are converted as soon as possible to API type, to avoid interface pollution
        const auto graphicsCommandList = commandList->as<D3D12CommandList>()->asGraphicsCommandList();
        graphicsCommandList->SetPipelineState(mPipelineState.Get());
        graphicsCommandList->SetGraphicsRootSignature(mRootSignature.Get());
    }

private:
    static D3D12_ROOT_SIGNATURE_FLAGS getRootSignatureFlags(bool hasVertexInputs);

    ComPtr<ID3D12RootSignature> mRootSignature;
    ComPtr<ID3D12PipelineState> mPipelineState;
    PipelineType                mPipelineType;
    D3D12Device*                mDevice;
    const char*                 mName;
};