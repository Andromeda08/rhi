#pragma once

#include "D3D12Core.hpp"
#include "D3D12Device.hpp"
#include "RHI/RHITexture.hpp"

struct D3D12TextureCreateInfo
{
    Size2D          size;
    Format          format  {Format::R32G32B32A32Sfloat};
    bool            sampled {true};
    std::string     debugName;
    D3D12Device*    pDevice;
};

class D3D12Texture : public RHITexture
{
public:
    DISABLE_COPY_CTOR(D3D12Texture);
    explicit DEF_PRIMARY_CTOR(D3D12Texture, const D3D12TextureCreateInfo& createInfo);

    ~D3D12Texture() override;

    DXGI_FORMAT getFormat() const { return mFormat; }
    ID3D12Resource* getDSV() const { return mResource; }
    CD3DX12_CPU_DESCRIPTOR_HANDLE getDSVHandle() const
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(mDSVHeap->GetCPUDescriptorHandleForHeapStart());
        return dsvHandle;
    }

private:
    void createDSV();

    Size2D                          mSize;
    DXGI_FORMAT                     mFormat;
    D3D12MA::Allocation*            mAllocation;
    ID3D12Resource*                 mResource;

    D3D12_RESOURCE_STATES           mState { D3D12_RESOURCE_STATE_COMMON };

    ComPtr<ID3D12DescriptorHeap>    mDSVHeap;
    D3D12_DEPTH_STENCIL_VIEW_DESC   mDSViewDesc;
    ComPtr<ID3D12Resource>          mDSView;

    std::wstring                    mDebugName {};
    D3D12Device*                    mDevice;
};