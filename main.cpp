#include <iostream>

#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#include "D3D12RHI/D3D12RHI.hpp"
#include "WSI/Window.hpp"

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

    gRHI = D3D12RHI::createD3D12RHI();
    gRHI->init();

    return 0;
}
