#include <iostream>

#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"

int main()
{
    gRHI = std::make_shared<VulkanRHI>();
    gRHI->init();
    return 0;
}
