#pragma once

#include "Core/Base.hpp"
#include "RHI/Definitions.hpp"
#include "RHI/Macros.hpp"

// Forward Declarations
class D3D12Device;
class D3D12CommandQueue;
class D3D12CommandList;
class D3D12Swapchain;

// DirectX 12, Windows and related headers
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl/client.h>

#include <D3D12MemAlloc.h>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::WeakRef;

#define D3D12_CHECK(FN, ERROR_MSG)       \
if (auto result = FN; FAILED(result)) {  \
    throw std::runtime_error(ERROR_MSG); \
}

#pragma region "Logging"

#define D3D12_PREFIX "D3D12"
#define D3D12_STYLED_PREFIX styled(D3D12_PREFIX, fg(fmt::color::green_yellow))

#ifdef D3D12_VERBOSE_LOGGING
#define D3D12_VERBOSE(MESSAGE) fmt::println("[{}] {}", D3D12_STYLED_PREFIX, MESSAGE)
#else
#define D3D12_VERBOSE(MESSAGE)
#endif

#ifdef D3D12_DEBUGGING_ENABLED
    #define D3D12_DEBUG(MESSAGE) fmt::println("[{}] {}", D3D12_STYLED_PREFIX, MESSAGE)
#else
    #define D3D12_DEBUG(MESSAGE)
#endif

#define D3D12_PRINTLN(MESSAGE) fmt::println("[{}] {}", D3D12_STYLED_PREFIX, MESSAGE)

#pragma endregion

#pragma region "wstring utilities"

#define TO_WSTR(STRING) std::wstring(std::begin(STRING), std::end(STRING))

#define TO_LPCWSTR(STRING) TO_WSTR(STRING).c_str()

std::string to_string(const std::wstring& wstring);

#pragma endregion

#pragma region "RHI to D3D12 type conversion"

inline D3D12_CULL_MODE toD3D12(const CullMode cullMode)
{
    switch (cullMode)
    {
        case CullMode::None:
            return D3D12_CULL_MODE_NONE;
        case CullMode::Front:
            return D3D12_CULL_MODE_FRONT;
        case CullMode::Back:
            return D3D12_CULL_MODE_BACK;
        default:
            throw std::runtime_error("Unsupported CullMode");
    }
}

#pragma endregion

#pragma region "D3D12 to RHI type conversion"

inline Rect2D toRHI(const CD3DX12_RECT& rect)
{
    return {
        .offset = Offset2D(rect.top, rect.left),
        .size = Size2D(rect.bottom, rect.right),
    };
}

inline Viewport toRHI(const CD3DX12_VIEWPORT& viewport)
{
    return {
        .x = viewport.TopLeftX,
        .y = viewport.TopLeftY,
        .width = viewport.Width,
        .height = viewport.Height,
        .minDepth = viewport.MinDepth,
        .maxDepth = viewport.MaxDepth,
    };
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

#pragma endregion
