#pragma once

#include "D3D12Core.hpp"

class D3D12CommandQueue;

struct D3D12CreateDescriptorHeapParams
{
    D3D12_DESCRIPTOR_HEAP_DESC    desc {};
    ComPtr<ID3D12DescriptorHeap>& descriptorHeap;
    std::string                   debugName;
};

struct D3D12CreateRenderTargetViewParams
{
    ComPtr<ID3D12Resource>&         rtv;
    D3D12_RENDER_TARGET_VIEW_DESC*  rtvDesc = nullptr;
    CD3DX12_CPU_DESCRIPTOR_HANDLE   cpuHandle;
    std::string                     debugName;
};

struct D3D12CreateDepthStencilViewParams
{
    ID3D12Resource*                 texture;
    D3D12_DEPTH_STENCIL_VIEW_DESC*  dsvDesc;
    CD3DX12_CPU_DESCRIPTOR_HANDLE   cpuHandle;
    std::string                     debugName;
};

class D3D12Device
{
public:
    DISABLE_COPY_CTOR(D3D12Device);
    explicit DEF_PRIMARY_CTOR(D3D12Device, const ComPtr<IDXGIAdapter1>& adapter);

    /**
     * Internal D3D12 Wrappers
     */
    void makeRootSignature(ID3D12RootSignature** ppRootSignature, D3D12_ROOT_SIGNATURE_FLAGS flags) const;

    void createGraphicsPSO(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& graphicsPipelineStateDesc, ComPtr<ID3D12PipelineState>& pipelineState) const;

    void createFence(uint64_t initialValue, D3D12_FENCE_FLAGS flags, ComPtr<ID3D12Fence>& fence) const;

    D3D12MA::Allocator* getAllocator() const { return mAllocator; }

    /**
     * Command Queues
     */
    D3D12CommandQueue* getDirectQueue() const { return mDirectQueue.get(); }

    /**
     * Descriptor Heaps
     */
    void     createDescriptorHeap(const D3D12CreateDescriptorHeapParams& params) const;
    uint32_t getDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) const;

    void     createRenderTargetView(const D3D12CreateRenderTargetViewParams& params) const;
    void     createDepthStencilView(const D3D12CreateDepthStencilViewParams& params) const;

private:
    ComPtr<IDXGIAdapter1> mAdapter;
    ComPtr<ID3D12Device>  mDevice;
    D3D12MA::Allocator*   mAllocator;

    std::unique_ptr<D3D12CommandQueue> mDirectQueue;

    std::string mAdapterName {"No Adapter"};
    DXGI_ADAPTER_DESC1 mAdapterDesc {};
};