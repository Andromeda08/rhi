#include "D3D12Swapchain.hpp"

D3D12Swapchain::D3D12Swapchain(const D3D12SwapchainParams& params)
: mSize(params.window->framebufferSize())
, mImageCount(params.imageCount)
, mDevice(params.device)
, mWindow(params.window)
{
    DXGI_SWAP_CHAIN_DESC1 swapchainDesc {};
    swapchainDesc.BufferCount       = mImageCount;
    swapchainDesc.Width             = mSize.width;
    swapchainDesc.Height            = mSize.height;
    swapchainDesc.Format            = mFormat;
    swapchainDesc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.SampleDesc.Count  = 1;

    ComPtr<IDXGISwapChain1> swapchain1;
    D3D12_CHECK(
        params.factory->CreateSwapChainForHwnd(
            params.device->getDirectQueue()->getQueueHandle().Get(),
            mWindow->getWin32Handle(),
            &swapchainDesc, nullptr, nullptr, &swapchain1),
        "Failed to create D3D12 Swapchain."
    );

    D3D12_CHECK(swapchain1.As(&mSwapchain), "Failed to cast IDXGISwapChain1 to IDXGISwapChain4");

    mDevice->createDescriptorHeap({
        .desc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
            .NumDescriptors = mImageCount,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        },
        .descriptorHeap = mRTVDescriptorHeap,
        .debugName = "Swapchain Descriptor Heap",
    });

    // TODO: Result obj. or remain this way?
    mRTVSize = mDevice->getDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    mRTVs.resize(mImageCount);
    for (size_t i = 0; i < mImageCount; i++)
    {
        D3D12_CHECK(
            mSwapchain->GetBuffer(i, IID_PPV_ARGS(&mRTVs[i])),
            fmt::format("Failed to get Swapchain RTV {}", i)
        );
        mDevice->createRenderTargetView({
            .rtv = mRTVs[i],
            .cpuHandle = rtvHandle,
            .debugName = fmt::format("Swapchain RTV {}", i)
        });
        rtvHandle.Offset(1, mRTVSize);
    }

    mViewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(mSize.width), static_cast<float>(mSize.height));
    mScissor  = CD3DX12_RECT(0, 0, mSize.width, mSize.height);
}

std::shared_ptr<D3D12Swapchain> D3D12Swapchain::createD3D12Swapchain(const D3D12SwapchainParams& params)
{
    return std::make_shared<D3D12Swapchain>(params);
}


uint32_t D3D12Swapchain::getNextFrameIndex(uint32_t currentFrame) const
{
    return mSwapchain->GetCurrentBackBufferIndex();
}

void D3D12Swapchain::present() const
{
    PRINT_PLACEHOLDER("D3D12Swapchain::present()");
}
