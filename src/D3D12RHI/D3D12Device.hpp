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

class D3D12Device
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12Device, const ComPtr<IDXGIAdapter1>& adapter);

    /**
     * Command Queues
     */
    std::shared_ptr<D3D12CommandQueue> getDirectQueue() const { return mDirectQueue; }

    /**
     * Descriptor Heaps
     */
    void     createDescriptorHeap(const D3D12CreateDescriptorHeapParams& params) const;
    uint32_t getDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType) const;

    void     createRenderTargetView(const D3D12CreateRenderTargetViewParams& params) const;

private:
    ComPtr<IDXGIAdapter1> mAdapter;
    ComPtr<ID3D12Device> mDevice;

    std::shared_ptr<D3D12CommandQueue> mDirectQueue;

    std::string mAdapterName {"No Adapter"};
    DXGI_ADAPTER_DESC1 mAdapterDesc {};
};