#pragma once

#include <VulkanRHI/VulkanRHI.hpp>

#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

inline std::unique_ptr<DynamicRHI> rhiFactory(const RHIInterfaceType api, RHIWindow* pWindow)
{
    if (api == RHIInterfaceType::Vulkan)
    {
        return VulkanRHI::createVulkanRHI({
            .pWindow = pWindow
        });
    }
    if (api == RHIInterfaceType::D3D12)
    {
#ifdef __APPLE__
        fmt::println("[{}] {}",
            styled("Void", fg(fmt::color::medium_purple)),
            fmt::format("{} Backend not supported on {}",
                styled("D3D12", fg(fmt::color::green_yellow)),
                styled("macOS", fg(fmt::color::medium_purple))));
        throw std::exception();
#else
        return D3D12RHI::createD3D12RHI({
            .pWindow = pWindow,
        });
#endif
    }
    throw std::exception();
}
