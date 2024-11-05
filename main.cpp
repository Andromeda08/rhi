#include <iostream>

#include "WSI/Window.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

std::unique_ptr<Window> gWindow;

int main(int argc, char** argv)
{
    auto api = RHIInterfaceType::Vulkan;
    if (argc >= 2)
    {
        const std::string apiArg = argv[1];

        if (apiArg == "D3D12")  api = RHIInterfaceType::D3D12;
        else                    api = RHIInterfaceType::Vulkan;
    }

    const auto windowCreateInfo = WindowCreateInfo {
        .resolution = { 1280, 720 },
        .title = "RHI Example Window",
    };
    gWindow = std::make_unique<Window>(windowCreateInfo);

    switch (api)
    {
        case RHIInterfaceType::Vulkan: {
            gRHI = VulkanRHI::createVulkanRHI({
                .pWindow = gWindow.get(),
            });
            break;
        }
        case RHIInterfaceType::D3D12: {
            gRHI = D3D12RHI::createD3D12RHI({
                .pWindow = gWindow.get(),
            });
            break;
        }
        default:
            return 0;
    }

    while (!gWindow->shouldClose())
    {
        glfwPollEvents();
    }

    return 0;
}
