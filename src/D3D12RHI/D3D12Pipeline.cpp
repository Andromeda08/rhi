#include "D3D12Pipeline.hpp"

#include <ranges>

D3D12Pipeline::D3D12Pipeline(D3D12PipelineCreateInfo& createInfo)
: RHIPipeline()
, mPipelineType(createInfo.pipelineType), mDevice(createInfo.device), mName(createInfo.debugName)
{
    /**
     * Root Signature
     */
    const auto rootSignatureFlags = getRootSignatureFlags(createInfo.inputElements.empty());
    mDevice->makeRootSignature(&mRootSignature, rootSignatureFlags);

    const auto rootSignatureName = fmt::format("{} RootSignature", mName);
    D3D12_CHECK(mRootSignature->SetName(TO_LPCWSTR(rootSignatureName)), "Failed to name ID3D12RootSignature");

    /**
     * Pipeline State
     */
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.pRootSignature = mRootSignature.Get();

    psoDesc.InputLayout = {
        .pInputElementDescs = createInfo.inputElements.data(),
        .NumElements = static_cast<UINT>(createInfo.inputElements.size()),
    };
    psoDesc.RasterizerState = createInfo.graphicsPiplineState.rasterizerState;
    psoDesc.RasterizerState.FrontCounterClockwise = true;
    psoDesc.BlendState = createInfo.graphicsPiplineState.blendState;

    // TODO: Depth and Stencil disabled
    psoDesc.DepthStencilState = createInfo.graphicsPiplineState.depthStencilState;
    psoDesc.DepthStencilState.DepthEnable = false; // true in reference
    // psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    psoDesc.DepthStencilState.StencilEnable = false;

    psoDesc.SampleDesc.Count = 1u;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    auto renderTargets = createInfo.renderPass->getRenderTargets(0);
    psoDesc.NumRenderTargets = renderTargets.size();

    for (const auto& [i, rtv] : std::views::enumerate((renderTargets)))
    {
        psoDesc.RTVFormats[i] = rtv.format;
    }

    /**
     * Shaders
     */
    std::vector<ComPtr<ID3DBlob>> shaderBlobs(createInfo.shadersCreateInfos.size());
    for (const auto& [i, shaderInfo] : std::views::enumerate(createInfo.shadersCreateInfos))
    {
        const auto pathStr = std::string(shaderInfo.filePath);
        const auto path = TO_WSTR(pathStr);
        D3D12_CHECK(D3DReadFileToBlob(path.c_str(), &shaderBlobs[i]), "Failed to read shader bytecode");

        const auto blob = CD3DX12_SHADER_BYTECODE(shaderBlobs[i].Get());
        switch (shaderInfo.shaderStage)
        {
            case ShaderStage::Vertex:{
                psoDesc.VS = blob;
                break;
            }
            case ShaderStage::Fragment:{
                psoDesc.PS = blob;
                break;
            }
            default: {
                fmt::println("Unsupported shader type");
            }
        }
    }

    // Create PSO
    mDevice->createGraphicsPSO(psoDesc, mPipelineState);
}

std::unique_ptr<D3D12Pipeline> D3D12Pipeline::createD3D12Pipeline(D3D12PipelineCreateInfo& createInfo)
{
    return std::make_unique<D3D12Pipeline>(createInfo);
}

D3D12Pipeline::~D3D12Pipeline()
{
}

D3D12_ROOT_SIGNATURE_FLAGS D3D12Pipeline::getRootSignatureFlags(const bool hasVertexInputs)
{
    return hasVertexInputs
        ? D3D12_ROOT_SIGNATURE_FLAG_NONE
        : D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}
