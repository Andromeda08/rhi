#pragma once

#include "D3D12Core.hpp"

using D3D12DevicePtr = std::shared_ptr<class D3D12Device>;

class D3D12RHI : public DynamicRHI
{
public:
    DEF_PRIMARY_CTOR(D3D12RHI);

    ~D3D12RHI() override = default;

    void init() override;

    ComPtr<IDXGIFactory4> getFactory() const { return mFactory; }
    D3D12DevicePtr getDevice() const { return mDevice; }

private:
    void createFactory();

    ComPtr<IDXGIAdapter1> selectAdapter() const;

    void createDevice();

    ComPtr<IDXGIFactory4> mFactory;
    ComPtr<ID3D12Debug> mDebug;

    D3D12DevicePtr mDevice;
};
