#include "D3D12Buffer.hpp"

#include "D3D12CommandList.hpp"
#include "D3D12Device.hpp"
#include "RHI/RHICommandList.hpp"

D3D12_HEAP_TYPE getD3D12HeapType(const RHIBufferType bufferType)
{
    switch (bufferType)
    {
        case Index:
        case Storage:
        case Uniform:
        case Vertex:
            return D3D12_HEAP_TYPE_DEFAULT;
        case Staging:
            return D3D12_HEAP_TYPE_UPLOAD;
        default:
            throw std::runtime_error("Invalid buffer type");
    }
}

D3D12Buffer::D3D12Buffer(const D3D12BufferCreateInfo& createInfo)
: RHIBuffer()
, mSize(createInfo.bufferSize)
, mBufferType(createInfo.bufferType)
, mDebugName(createInfo.debugName)
, mDevice(createInfo.pDevice)
{
    mHeapType = getD3D12HeapType(createInfo.bufferType);

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = mHeapType;

    D3D12_RESOURCE_DESC resourceDesc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0,
        .Width = mSize,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = {
            .Count = 1,
            .Quality = 0,
        },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
    };

    const auto result = mDevice->getAllocator()->CreateResource(&allocationDesc, &resourceDesc,
        mState, nullptr, &mAllocation, IID_NULL, nullptr);

    if (!SUCCEEDED(result))
    {
        throw std::runtime_error(fmt::format(
            "Failed to allocate D3D12 Resource ({})",
            std::system_category().message(result)));
    }

    mResource = mAllocation->GetResource();
    mAddress = mResource->GetGPUVirtualAddress();

    D3D12_CHECK(mResource->SetName(mDebugName), "Failed to name D3D12 Resource");
}

std::unique_ptr<D3D12Buffer> D3D12Buffer::createD3D12Buffer(const D3D12BufferCreateInfo& createInfo)
{
    return std::make_unique<D3D12Buffer>(createInfo);
}

D3D12Buffer::~D3D12Buffer()
{
    mAllocation->Release();
}

void D3D12Buffer::setData(const void* pData, const uint64_t dataSize) const
{
    if (mHeapType != D3D12_HEAP_TYPE_UPLOAD)
    {
        fmt::println("[{}] {} {} {}",
            D3D12_STYLED_PREFIX,
            styled("Warning: For buffers that weren't allocated on", fg(fmt::color::light_yellow)),
            styled("D3D12_HEAP_TYPE_UPLOAD", fg(fmt::color::cornflower_blue)),
            styled("data upload should be done via a CommandList, this D3D12Buffer::setData() call will have no effect.", fg(fmt::color::light_yellow)));
        return;
    }

    void* mappedMemory;
    D3D12_CHECK(mResource->Map(0, nullptr, &mappedMemory), "Failed to map memory");
    memcpy(mappedMemory, pData, dataSize);
    mResource->Unmap(0, nullptr);
}

void D3D12Buffer::uploadData(const RHIBufferUploadInfo& uploadInfo)
{
    // D3D12_PRINTLN("D3D12Buffer::uploadData() not implemented");

    D3D12Buffer* stagingBuffer = uploadInfo.pStagingBuffer->as<D3D12Buffer>();

    auto* d3d12List = uploadInfo.pCommandList->as<D3D12CommandList>();
    auto* commandList = d3d12List->asGraphicsCommandList();

    const auto toCopyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        mResource,
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST
    );
    commandList->ResourceBarrier(1, &toCopyBarrier);

    commandList->CopyBufferRegion(
        mResource, 0,
        stagingBuffer->mResource, 0,
        uploadInfo.dataSize);

    const auto toGenericBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        mResource,
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );
    commandList->ResourceBarrier(1, &toGenericBarrier);
}

D3D12_VERTEX_BUFFER_VIEW& D3D12Buffer::getVertexBufferView()
{
    if (mBufferType != Vertex)
    {
        const auto msg = "getVertexBufferView() can only be called on Vertex buffers";
        D3D12_PRINTLN(msg);
        throw std::runtime_error(msg);
    }

    mVertexBufferView = {
        .BufferLocation = mAddress,
        .SizeInBytes = static_cast<UINT>(mSize),
        .StrideInBytes = 32u,
    };
    return mVertexBufferView;
}

D3D12_INDEX_BUFFER_VIEW& D3D12Buffer::getIndexBufferView()
{
    if (mBufferType != Index)
    {
        const auto msg = "getIndexBufferView() can only be called on Index buffers";
        D3D12_PRINTLN(msg);
        throw std::runtime_error(msg);
    }

    mIndexBufferView = {
        .BufferLocation = mAddress,
        .SizeInBytes = static_cast<UINT>(mSize),
        .Format = DXGI_FORMAT_R32_UINT,
    };
    return mIndexBufferView;
}
