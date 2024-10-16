#include <iostream>

#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"

int main()
{
    g_rhi = std::make_shared<VulkanRHI>();
    g_rhi->init();
    return 0;
}
