#include "D3D12RHI.hpp"

#include "D3D12Device.hpp"

D3D12RHI::D3D12RHI()
: DynamicRHI()
{
}

std::shared_ptr<D3D12RHI> D3D12RHI::createD3D12RHI()
{
    return std::make_shared<D3D12RHI>();
}

void D3D12RHI::init()
{
    createFactory();

    createDevice();

    #ifdef D3D12_DEBUGGING_ENABLED
    fmt::println("[Info] RHI initialized, using API: {}",
        styled("D3D12", fg(fmt::color::green_yellow)));
    #endif
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

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
        {
            break;
        }
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