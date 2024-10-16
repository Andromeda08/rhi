#include "VulkanRHI.hpp"

VulkanRHI::VulkanRHI()
: DynamicRHI()
{
    fmt::println("Created {} RHI", fmt::styled("Vulkan", fmt::fg(fmt::color::indian_red)));
}

void VulkanRHI::init()
{
    create_instance();
}

void VulkanRHI::create_instance()
{
    auto create_info = vk::InstanceCreateInfo();
}

std::shared_ptr<DynamicRHI> VulkanRHI::create_vulkan_rhi()
{
    return std::make_shared<VulkanRHI>();
}