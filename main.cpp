#include <iostream>

#include "WSI/Window.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

std::unique_ptr<Window> gWindow;

int main()
{
    const auto windowCreateInfo = WindowCreateInfo {
        .resolution = { 1280, 720 },
        .title = "RHI Example Window",
    };

    gWindow = std::make_unique<Window>(windowCreateInfo);
    gRHI = VulkanRHI::createVulkanRHI({
        .pWindow = gWindow.get(),
    });

    gWindow = std::make_unique<Window>(windowCreateInfo);
    gRHI = D3D12RHI::createD3D12RHI({
        .pWindow = gWindow.get(),
    });

    //std::cout << std::string(48, '=') << std::endl;

    return 0;
}
