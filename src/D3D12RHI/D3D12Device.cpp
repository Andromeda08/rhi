#include "D3D12Device.hpp"

D3D12Device::D3D12Device(const ComPtr<IDXGIAdapter1>& adapter)
: mAdapter(adapter)
{
    D3D12_CHECK(D3D12CreateDevice(mAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&mDevice)),
        "Failed to create D3D12Device");

    D3D12_CHECK(mAdapter->GetDesc1(&mAdapterDesc), "Failed to get adapter description");
    mAdapterName = to_string(mAdapterDesc.Description);

    D3D12_CHECK(mDevice->SetName(NDX_TO_LPCWSTR(mAdapterName)), "Failed to name D3D12Device");

    #ifdef D3D12_DEBUGGING_ENABLED
    fmt::println("[Info] Using Adapter: {}",
        styled(mAdapterName, fg(fmt::color::honey_dew)));
    #endif
}

std::shared_ptr<D3D12Device> D3D12Device::createD3D12Device(const ComPtr<IDXGIAdapter1>& adapter)
{
    return std::make_shared<D3D12Device>(adapter);
}
