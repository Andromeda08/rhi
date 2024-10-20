#pragma once

#include "Core/Base.hpp"
#include "RHI/DynamicRHI.hpp"

/**
 * DirectX 12, Windows and related headers
 */
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::WeakRef;

/**
 * String and converters
 */
#define NDX_TO_WSTR(STRING) std::wstring(std::begin(STRING), std::end(STRING))

#define NDX_TO_LPCWSTR(STRING) NDX_TO_WSTR(STRING).c_str()

std::string to_string(const std::wstring& wstring);

/**
 * Macros
 */
#define D3D12_CHECK(FN, ERROR_MSG)       \
if (auto result = FN; FAILED(result)) {  \
    throw std::runtime_error(ERROR_MSG); \
}