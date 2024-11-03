#include <iostream>

#include "WSI/Window.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

std::shared_ptr<Window> gWindow;

int main()
{
    const auto windowCreateInfo = WindowCreateInfo {
        .resolution = { 1280, 720 },
        .title = "RHI Example Window",
    };
    gWindow = Window::createWindow(windowCreateInfo);

    gRHI = VulkanRHI::createVulkanRHI();
    gRHI->init(gWindow);

    std::cout << std::string(48, '=') << std::endl;

#ifdef D3D12_RHI_ENABLED
    gRHI = D3D12RHI::createD3D12RHI();
    gRHI->init(gWindow);
#endif

    return 0;
}
