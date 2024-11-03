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

    #ifdef D3D12_DEBUGGING_ENABLED
    fmt::println("[Info] Using Adapter: {}",
        styled(mAdapterName, fg(fmt::color::honey_dew)));
    #endif

    mDirectQueue = D3D12CommandQueue::createD3D12CommandQueue({
        .device = mDevice,
        .type = RHICommandQueueType::Graphics,
        .commandListCount = 2,
    });
}

std::shared_ptr<D3D12Device> D3D12Device::createD3D12Device(const ComPtr<IDXGIAdapter1>& adapter)
{
    return std::make_shared<D3D12Device>(adapter);
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
