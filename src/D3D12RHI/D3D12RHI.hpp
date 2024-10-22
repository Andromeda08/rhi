#pragma once

#include "RHI/DynamicRHI.hpp"
#include "RHI/IWindow.hpp"
#include "D3D12Core.hpp"

using D3D12DevicePtr = std::shared_ptr<class D3D12Device>;

class D3D12RHI : public DynamicRHI
{
public:
    DEF_PRIMARY_CTOR(D3D12RHI);

    ~D3D12RHI() override = default;

    void init(const std::shared_ptr<IRHIWindow>& window) override;

    std::shared_ptr<RHICommandQueue> getGraphicsQueue() override { return nullptr; }

    RHIInterfaceType getType() const override { return RHIInterfaceType::D3D12; }

    ComPtr<IDXGIFactory4> getFactory() const { return mFactory; }
    D3D12DevicePtr getDevice() const { return mDevice; }

private:
    void createFactory();

    ComPtr<IDXGIAdapter1> selectAdapter() const;

    void createDevice();

private:
    ComPtr<IDXGIFactory4> mFactory;
    ComPtr<ID3D12Debug> mDebug;

    D3D12DevicePtr mDevice;
};
