#include <iostream>

#include "WSI/Window.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

WindowPtr gWindow;

int main()
{
    const auto windowCreateInfo = WindowCreateInfo {
        .resolution = { 1600, 900 },
        .title = "RHI Example Window",
    };
    // gWindow = Window::createWindow(windowCreateInfo);

    gRHI = VulkanRHI::createVulkanRHI();
    gRHI->init();

    std::cout << std::string(48, '=') << std::endl;

#ifdef D3D12_RHI_ENABLED
    gRHI = D3D12RHI::createD3D12RHI();
    gRHI->init();
#endif

    return 0;
}
