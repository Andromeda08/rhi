#include <iostream>

#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#include "D3D12RHI/D3D12RHI.hpp"

int main()
{
    gRHI = VulkanRHI::createVulkanRHI();
    gRHI->init();

    std::cout << std::string(48, '=') << std::endl;

    gRHI = D3D12RHI::createD3D12RHI();
    gRHI->init();

    return 0;
}
