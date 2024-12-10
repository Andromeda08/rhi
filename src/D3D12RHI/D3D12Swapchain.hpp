#pragma once

#include "D3D12CommandQueue.hpp"
#include "D3D12Core.hpp"
#include "D3D12Device.hpp"
#include "D3D12RenderPass.hpp"
#include "RHI/RHISwapchain.hpp"
#include "RHI/RHIWindow.hpp"

struct D3D12SwapchainParams
{
    RHIWindow*             window;
    D3D12Device*           device;
    ComPtr<IDXGIFactory4>& factory;
    uint32_t               imageCount {2};
};

class D3D12Swapchain : public RHISwapchain
{
public:
    explicit DEF_PRIMARY_CTOR(D3D12Swapchain, const D3D12SwapchainParams& params);

    ~D3D12Swapchain() override = default;

    uint32_t getNextFrameIndex(uint32_t currentFrame) const override;

    Rect2D   getScissor()           override { return toRHI(mScissor);}
    Viewport getViewport()          override { return toRHI(mViewport); }
    Size2D   getSize()        const override { return mSize; }
    float    getAspectRatio() const override { return mAspectRatio; }
    Format   getFormat()      const override { return toRHI(mFormat); }
    uint32_t getFrameCount()        override { return mImageCount; }

    void setScissorViewport(RHICommandList* commandList) const override;

    D3D12RenderTarget getRenderTarget(const uint32_t i) const
    {
        if (i > mRTVs.size())
        {
            throw std::out_of_range(fmt::format("Index {} out of range", i));
        }

        const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), static_cast<int>(i), mRTVSize);

        return {
            .format = mFormat,
            .rtv = mRTVs[i].Get(),
            .rtvHandle = rtvHandle,
            .initialState = D3D12_RESOURCE_STATE_PRESENT,
        };
    }

    UINT getCurrentBackBufferIndex() const
    {
        return mSwapchain->GetCurrentBackBufferIndex();
    }

    void present(const uint32_t syncInterval = 1, const uint32_t flags = 0) const
    {
        D3D12_CHECK(mSwapchain->Present(syncInterval, flags), "Failed to present to Swapchain");
    }

private:
    ComPtr<IDXGISwapChain4>             mSwapchain;

    CD3DX12_VIEWPORT                    mViewport {};
    CD3DX12_RECT                        mScissor {};
    DXGI_FORMAT                         mFormat { DXGI_FORMAT_B8G8R8A8_UNORM };
    Size2D                              mSize {};
    float                               mAspectRatio {};
    const uint32_t                      mImageCount {0};

    uint32_t                            mRTVSize {0};
    ComPtr<ID3D12DescriptorHeap>        mRTVDescriptorHeap;
    std::vector<ComPtr<ID3D12Resource>> mRTVs;

    RHIWindow*                          mWindow;
    D3D12Device*                        mDevice;
};