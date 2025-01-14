#pragma once

#include "RHI/DynamicRHI.hpp"
#include "RHI/RHIWindow.hpp"
#include "RHI/Macros.hpp"
#include "D3D12Core.hpp"
#include "D3D12Device.hpp"
#include "D3D12Swapchain.hpp"

struct D3D12RHICreateInfo
{
    RHIWindow* pWindow;
};

class D3D12RHI final : public DynamicRHI
{
public:
    DISABLE_COPY_CTOR(D3D12RHI);
    explicit DEF_PRIMARY_CTOR(D3D12RHI, const D3D12RHICreateInfo& createInfo);

    ~D3D12RHI() override = default;

    void waitIdle() override;

    Frame beginFrame(const RHIFrameBeginInfo& frameBeginInfo) override;

    void submitFrame(const Frame& frame) override;

    std::unique_ptr<RHIFramebuffer> createFramebuffer(const RHIFramebufferCreateInfo& createInfo) override;

    std::unique_ptr<RHIRenderPass> createRenderPass(const RHIRenderPassCreateInfo& createInfo) override;

    std::unique_ptr<RHIPipeline> createPipeline(const RHIPipelineCreateInfo& createInfo) override;

    std::unique_ptr<RHIBuffer> createBuffer(const RHIBufferCreateInfo& createInfo) override;

    std::unique_ptr<RHITexture> createTexture(const RHITextureCreateInfo& createInfo) override;


    RHICommandQueue* getGraphicsQueue() override;

    RHIInterfaceType      getType()      const override { return RHIInterfaceType::D3D12; }
    RHISwapchain*         getSwapchain() const override { return mSwapchain.get(); }
    ComPtr<IDXGIFactory4> getFactory()   const { return mFactory; }
    D3D12Device*          getDevice()    const { return mDevice.get(); }

private:
    void                    createFactory();
    ComPtr<IDXGIAdapter1>   selectAdapter() const;
    void                    createDevice();

private:
    ComPtr<IDXGIFactory4>           mFactory;
    ComPtr<ID3D12Debug>             mDebug;

    std::unique_ptr<D3D12Device>    mDevice;
    std::unique_ptr<D3D12Swapchain> mSwapchain;

    std::vector<uint64_t>           mFenceValues;
    ComPtr<ID3D12Fence>             mFence;
    HANDLE                          mFenceEvent {nullptr};
    UINT                            mFrameIndex {0};
    uint32_t                        mFramesInFlight {2};
    uint32_t                        mCurrentFrame {0};
};
