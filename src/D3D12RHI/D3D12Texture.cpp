#include "D3D12Texture.hpp"

D3D12Texture::D3D12Texture(const D3D12TextureCreateInfo& createInfo)
: RHITexture()
, mSize(createInfo.size)
, mFormat(toD3D12(createInfo.format))
, mDevice(createInfo.pDevice)
, mDebugName(TO_WSTR(createInfo.debugName))
{
    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_RESOURCE_DESC resourceDesc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Alignment = 0,
        .Width = mSize.width,
        .Height = mSize.height,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = mFormat,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0
        },
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
    };

    if (isDepthFormat(createInfo.format))
    {
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }

    const auto result = mDevice->getAllocator()->CreateResource(&allocationDesc, &resourceDesc,
        mState, nullptr, &mAllocation, IID_NULL, nullptr);

    if (!SUCCEEDED(result))
    {
        throw std::runtime_error(fmt::format(
            "Failed to allocate D3D12 Resource ({})",
            std::system_category().message(result)));
    }

    mResource = mAllocation->GetResource();

    D3D12_CHECK(mResource->SetName(mDebugName.c_str()), "Failed to name D3D12 Resource");
}

std::unique_ptr<D3D12Texture> D3D12Texture::createD3D12Texture(const D3D12TextureCreateInfo& createInfo)
{
    return std::make_unique<D3D12Texture>(createInfo);
}

D3D12Texture::~D3D12Texture()
{
    mAllocation->Release();
}
