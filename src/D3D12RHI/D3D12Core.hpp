#pragma once

#include "Core/Base.hpp"
#include "RHI/RHIDefinitions.hpp"

// Forward Declarations
class D3D12Device;
class D3D12CommandQueue;
class D3D12CommandList;
class D3D12Swapchain;

/**
 * DirectX 12, Windows and related headers
 */
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::WeakRef;

#define D3D12_PRINTLN(MESSAGE) \
    fmt::println("[{}] {}", styled("D3D12", fg(fmt::color::green_yellow)), MESSAGE)

/**
 * String and converters
 */
#define TO_WSTR(STRING) std::wstring(std::begin(STRING), std::end(STRING))

#define TO_LPCWSTR(STRING) TO_WSTR(STRING).c_str()

std::string to_string(const std::wstring& wstring);

/**
 * Macros
 */
#define D3D12_CHECK(FN, ERROR_MSG)       \
if (auto result = FN; FAILED(result)) {  \
    throw std::runtime_error(ERROR_MSG); \
}

/**
 * D3D12 -> RHI Conversion
 */
inline Rect2D toRHI(const CD3DX12_RECT& rect)
{
    return Rect2D(
        Size2D(rect.bottom, rect.right),
        Offset2D(rect.top, rect.left)
    );
}

inline Viewport toRHI(const CD3DX12_VIEWPORT& viewport)
{
    return Viewport(
        viewport.TopLeftX, viewport.TopLeftY,
        viewport.Width, viewport.Height,
        viewport.MinDepth, viewport.MaxDepth
    );
}

inline Format toRHI(const DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return Format::B8G8R8A8Unorm;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            return Format::R32G32B32A32Sfloat;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return Format::R32G32B32Sfloat;
        case DXGI_FORMAT_R32G32_FLOAT:
            return Format::R32G32Sfloat;
        case DXGI_FORMAT_R32_FLOAT:
            return Format::R32Sfloat;
        default:
            throw std::runtime_error("Unsupported Format");
    }
}