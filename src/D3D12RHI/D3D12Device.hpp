#pragma once

#include "D3D12Core.hpp"

class D3D12Device
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12Device, const ComPtr<IDXGIAdapter1>& adapter);

private:
    ComPtr<IDXGIAdapter1> mAdapter;
    ComPtr<ID3D12Device> mDevice;

    std::string mAdapterName {"No Adapter"};
    DXGI_ADAPTER_DESC1 mAdapterDesc {};
};