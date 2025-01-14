#include "D3D12RHI.hpp"

#include "D3D12Buffer.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12Device.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12Framebuffer.hpp"
#include "D3D12Pipeline.hpp"
#include "D3D12Swapchain.hpp"

D3D12RHI::D3D12RHI(const D3D12RHICreateInfo& createInfo)
: DynamicRHI()
{
    createFactory();

    createDevice();

    mSwapchain = D3D12Swapchain::createD3D12Swapchain({
        .window = createInfo.pWindow,
        .device = mDevice.get(),
        .factory = mFactory,
        .imageCount = 2,
    });

    /**
     * Create and Setup Frame Synchronization Fences
     */
    mDevice->createFence(0, D3D12_FENCE_FLAG_NONE, mFence);
    D3D12_CHECK(mFence->SetName(L"FrameSyncFence"), "Failed to name ID3D12Fence");

    mFenceEvent = CreateEvent(nullptr, false, false, nullptr);

    mFenceValues.resize(mSwapchain->getFrameCount());
    for (auto& fenceValue : mFenceValues)
    {
        fenceValue = 0;
    }

    mCurrentFrame = 0;

    D3D12_PRINTLN(fmt::format("{} RHI initialized", D3D12_STYLED_PREFIX));
}

std::unique_ptr<D3D12RHI> D3D12RHI::createD3D12RHI(const D3D12RHICreateInfo& createInfo)
{
    return std::make_unique<D3D12RHI>(createInfo);
}

void D3D12RHI::waitIdle()
{
    const auto queueHandle = mDevice->getDirectQueue()->getQueueHandle();
    D3D12_CHECK(queueHandle->Signal(mFence.Get(), mFenceValues[mFrameIndex]), "Failed to signal Fence");
    D3D12_CHECK(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent), "Fence set even on completion failed");

    WaitForSingleObjectEx(mFenceEvent, INFINITE, false);
    mFenceValues[mFrameIndex]++;
}

Frame D3D12RHI::beginFrame(const RHIFrameBeginInfo& frameBeginInfo)
{
    const uint64_t currentFence = mFenceValues[mFrameIndex];
    const auto queueHandle = mDevice->getDirectQueue()->getQueueHandle();
    D3D12_CHECK(queueHandle->Signal(mFence.Get(), currentFence), "Failed to signal Fence");

    mFrameIndex = mSwapchain->getCurrentBackBufferIndex();

    if (mFence->GetCompletedValue() < mFenceValues[mFrameIndex])
    {
        D3D12_CHECK(mFence->SetEventOnCompletion(mFenceValues[mFrameIndex], mFenceEvent),
            "Fence set even on completion failed");
        WaitForSingleObjectEx(mFenceEvent, INFINITE, false);
    }

    mFenceValues[mFrameIndex] = currentFence + 1;

    return {
        .mCurrentFrame = mCurrentFrame,
        .mAcquiredFrameIndex = mFrameIndex,
    };
}

void D3D12RHI::submitFrame(const Frame& frame)
{
    std::vector<ID3D12CommandList*> pCommandLists;
    for (const auto& commandList : frame.mCommandLists)
    {
        pCommandLists.push_back(commandList->as<D3D12CommandList>()->handle());
    }

    const auto queueHandle = mDevice->getDirectQueue()->getQueueHandle();
    queueHandle->ExecuteCommandLists(pCommandLists.size(), pCommandLists.data());

    mSwapchain->present();

    waitIdle();
    mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
}

std::unique_ptr<RHIFramebuffer> D3D12RHI::createFramebuffer(const RHIFramebufferCreateInfo& createInfo)
{
    return D3D12Framebuffer::createD3D12Framebuffer();
}

std::unique_ptr<RHIRenderPass> D3D12RHI::createRenderPass(const RHIRenderPassCreateInfo& createInfo)
{
    D3D12RenderPassCreateInfo d3d12CreateInfo = {};

    for (uint32_t i = 0; i < mFramesInFlight; i++)
    {
        std::vector<D3D12RenderTarget> frameRenderTargets;
        for (const auto& [j, colorAttachment] : std::views::enumerate(createInfo.colorAttachments))
        {
            if (std::holds_alternative<RHISwapchain*>(colorAttachment.attachmentSource))
            {
                auto renderTarget = std::get<RHISwapchain*>(colorAttachment.attachmentSource)->as<D3D12Swapchain>()->getRenderTarget(i);
                frameRenderTargets.push_back(renderTarget);
            }
        }

        d3d12CreateInfo.perFrameRenderTargets.push_back(frameRenderTargets);
    }
    
    return D3D12RenderPass::createD3D12RenderPass(d3d12CreateInfo);
}

std::unique_ptr<RHIPipeline> D3D12RHI::createPipeline(const RHIPipelineCreateInfo& createInfo)
{
    D3D12PipelineCreateInfo d3d12CreateInfo = {
        .inputElements = {},    // TODO: Replace with processed InputElements
        .shadersCreateInfos = createInfo.shaderCreateInfos,
        .renderPass = createInfo.renderPass->as<D3D12RenderPass>(),
        .pipelineType = createInfo.pipelineType,
        .graphicsPiplineState = D3D12GraphicsPipelineStateInfo().setCullMode(toD3D12(createInfo.graphicsPipelineState.cullMode)),
        .device = mDevice.get(),
        .debugName = createInfo.debugName,
    };

    return D3D12Pipeline::createD3D12Pipeline(d3d12CreateInfo);
}

std::unique_ptr<RHIBuffer> D3D12RHI::createBuffer(const RHIBufferCreateInfo& createInfo)
{
    return D3D12Buffer::createD3D12Buffer({
        .bufferSize = createInfo.bufferSize,
        .bufferType = createInfo.bufferType,
        .pDevice = mDevice.get(),
        .debugName = TO_LPCWSTR(createInfo.debugName),
    });
}

RHICommandQueue* D3D12RHI::getGraphicsQueue()
{
    return mDevice->getDirectQueue();
}

void D3D12RHI::createFactory()
{
    uint32_t factory_flags = 0;

    D3D12_CHECK(
        D3D12GetDebugInterface(IID_PPV_ARGS(&mDebug)),
        "Failed to initialize D3D12 Debug Interface");

    mDebug->EnableDebugLayer();
    factory_flags = DXGI_CREATE_FACTORY_DEBUG;

    D3D12_CHECK(
        CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&mFactory)),
        "Failed to create DXGI Factory");

    constexpr UUID D3D12ExperimentalShaderModels = {
        0x76f5573e,
        0xf13a,
        0x40f5,
        { 0xb2, 0x97, 0x81, 0xce, 0x9e, 0x18, 0x93, 0x3f }
    };

    D3D12_CHECK(
        D3D12EnableExperimentalFeatures(1, &D3D12ExperimentalShaderModels, nullptr, nullptr),
        "Failed to enable experimental D3D12 features");
}

ComPtr<IDXGIAdapter1> D3D12RHI::selectAdapter() const
{
    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory;
    if (!SUCCEEDED(mFactory->QueryInterface(IID_PPV_ARGS(&factory))))
    {
        throw std::runtime_error("Failed to get Factory6");
    }

    for (uint32_t i = 0;
         SUCCEEDED(factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
         ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        D3D12_CHECK(adapter->GetDesc1(&desc), "Failed to get adapter description");

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            continue;
        }

        auto result = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr);
        if (SUCCEEDED(result))
        {
            break;
        }
        D3D12_PRINTLN(std::system_category().message(result));
    }

    if (adapter.Get() == nullptr)
    {
        for (uint32_t i = 0; SUCCEEDED(mFactory->EnumAdapters1(i, &adapter)); ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            D3D12_CHECK(adapter->GetDesc1(&desc), "Failed to get adapter description");

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        throw std::runtime_error("Failed to find a suitable Adapter");
    }

    return adapter;
}

void D3D12RHI::createDevice()
{
    const auto adapter = selectAdapter();
    mDevice = D3D12Device::createD3D12Device(adapter);
}