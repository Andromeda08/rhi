#include "D3D12Core.hpp"

std::string to_string(const std::wstring& wstring)
{
    std::string result;
    std::ranges::transform(wstring, std::back_inserter(result), [] (const wchar_t c) {
        return static_cast<char>(c);
    });
    return result;
}
