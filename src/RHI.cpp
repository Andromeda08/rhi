#include "RHI.hpp"

#include <VulkanRHI/VulkanRHI.hpp>

#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

#ifdef __APPLE__
    #include <fmt/color.h>
    #include <fmt/format.h>
#endif

rhi_BEGIN_NAMESPACE;

std::unique_ptr<DynamicRHI> createRHI(const RHICreateInfo& rhiCreateInfo)
{
    if (rhiCreateInfo.apiType == RHIInterfaceType::Vulkan)
    {
        return VulkanRHI::createVulkanRHI({
            .pWindow = rhiCreateInfo.pWindow
        });
    }
    if (rhiCreateInfo.apiType == RHIInterfaceType::D3D12)
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
            .pWindow = rhiCreateInfo.pWindow,
        });
#endif
    }
    throw std::exception();
}

rhi_END_NAMESPACE;
