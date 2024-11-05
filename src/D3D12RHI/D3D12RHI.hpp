#pragma once

#include "RHI/DynamicRHI.hpp"
#include "RHI/IWindow.hpp"
#include "RHI/Macros.hpp"
#include "D3D12Core.hpp"
#include "D3D12Device.hpp"

struct D3D12RHICreateInfo
{
    IRHIWindow* pWindow;
};

class D3D12RHI : public DynamicRHI
{
public:
    DISABLE_COPY_CTOR(D3D12RHI);
    explicit DEF_PRIMARY_CTOR(D3D12RHI, const D3D12RHICreateInfo& createInfo);

    ~D3D12RHI() override = default;

    void waitIdle() const override {}

    std::shared_ptr<RHICommandQueue> getGraphicsQueue() override;

    RHIInterfaceType getType() const override { return RHIInterfaceType::D3D12; }

    ComPtr<IDXGIFactory4>        getFactory() const { return mFactory; }
    std::shared_ptr<D3D12Device> getDevice()  const { return mDevice; }

private:
    void createFactory();

    ComPtr<IDXGIAdapter1> selectAdapter() const;

    void createDevice();

private:
    ComPtr<IDXGIFactory4>           mFactory;
    ComPtr<ID3D12Debug>             mDebug;

    std::shared_ptr<D3D12Device>    mDevice;
    std::shared_ptr<D3D12Swapchain> mSwapchain;
};
