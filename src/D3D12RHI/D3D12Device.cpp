#include "D3D12Device.hpp"
#include "D3D12CommandQueue.hpp"

D3D12Device::D3D12Device(const ComPtr<IDXGIAdapter1>& adapter)
: mAdapter(adapter)
{
    D3D12_CHECK(D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice)),
        "Failed to create D3D12Device");

    D3D12_CHECK(mAdapter->GetDesc1(&mAdapterDesc), "Failed to get adapter description");
    mAdapterName = to_string(mAdapterDesc.Description);

    D3D12_CHECK(mDevice->SetName(TO_LPCWSTR(mAdapterName)), "Failed to name D3D12Device");

    D3D12_PRINTLN(fmt::format("Using Adapter: {}", mAdapterName));

    mDirectQueue = D3D12CommandQueue::createD3D12CommandQueue({
        .device = mDevice.Get(),
        .type = RHICommandQueueType::Graphics,
        .commandListCount = 2,
    });

    D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
    allocatorDesc.pDevice = mDevice.Get();
    allocatorDesc.pAdapter = mAdapter.Get();
    allocatorDesc.Flags = D3D12MA::ALLOCATOR_FLAG_MSAA_TEXTURES_ALWAYS_COMMITTED | D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;

    D3D12_CHECK(
        D3D12MA::CreateAllocator(&allocatorDesc, &mAllocator),
        "Failed to create D3D12 Allocator");

    D3D12_VERBOSE("Created D3D12Device");
}

std::unique_ptr<D3D12Device> D3D12Device::createD3D12Device(const ComPtr<IDXGIAdapter1>& adapter)
{
    return std::make_unique<D3D12Device>(adapter);
}

void D3D12Device::makeRootSignature(ID3D12RootSignature** ppRootSignature, D3D12_ROOT_SIGNATURE_FLAGS flags) const
{
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(0, nullptr, 0, nullptr, flags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    D3D12_CHECK(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error),
        "Failed to serialize RootSignature");

    D3D12_CHECK(mDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(ppRootSignature)),
        "Failed to create RootSignature");
}

void D3D12Device::createGraphicsPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc,
    ComPtr<ID3D12PipelineState>& pipelineState) const
{
    D3D12_CHECK(mDevice->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState)),
        "Failed to create GraphicsPipelineState");
}

void D3D12Device::createFence(const uint64_t initialValue, const D3D12_FENCE_FLAGS flags, ComPtr<ID3D12Fence>& fence) const
{
    D3D12_CHECK(mDevice->CreateFence(initialValue, flags, IID_PPV_ARGS(&fence)), "Failed to create Fence");
}

void D3D12Device::createDescriptorHeap(const D3D12CreateDescriptorHeapParams& params) const
{
    D3D12_CHECK(
        mDevice->CreateDescriptorHeap(&params.desc, IID_PPV_ARGS(&params.descriptorHeap)),
        "Failed to create Descriptor Heap"
    );

    #ifdef D3D12_DEBUGGING_ENABLED
    D3D12_CHECK(
        params.descriptorHeap->SetName(TO_LPCWSTR(params.debugName)),
        "Failed to name Descriptor Heap"
    );
    #endif
}

uint32_t D3D12Device::getDescriptorHandleIncrementSize(const D3D12_DESCRIPTOR_HEAP_TYPE heapType) const
{
    return mDevice->GetDescriptorHandleIncrementSize(heapType);
}

void D3D12Device::createRenderTargetView(const D3D12CreateRenderTargetViewParams& params) const
{
    mDevice->CreateRenderTargetView(params.rtv.Get(), params.rtvDesc, params.cpuHandle);

    #ifdef D3D12_DEBUGGING_ENABLED
    D3D12_CHECK(
        params.rtv->SetName(TO_LPCWSTR(params.debugName)),
        "Failed to name Render Target View"
    );
    #endif
}

void D3D12Device::createDepthStencilView(const D3D12CreateDepthStencilViewParams& params) const
{
    mDevice->CreateDepthStencilView(params.texture, params.dsvDesc, params.cpuHandle);
}
