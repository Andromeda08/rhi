#pragma once

#include <fmt/format.h>
#include <fmt/color.h>
#include <vulkan/vulkan.hpp>

#include "RHI/DynamicRHI.hpp"

// TODO: Change to fwd decl.
class VulkanDevice {};

class VulkanRHI : public DynamicRHI
{
public:
    VulkanRHI();

    void init() override;

    RHIInterfaceType get_type() const override { return RHIInterfaceType::Vulkan; }

    static std::shared_ptr<DynamicRHI> create_vulkan_rhi();

private:
    void create_instance();
    void select_physical_device();
    void create_device();

    vk::Instance m_instance;
    std::vector<const char*> m_instance_extensions;
    std::vector<const char*> m_instance_layers;

    std::shared_ptr<VulkanDevice> m_device;

#if VULKAN_DEBUGGING_ENABLED
    vk::DebugUtilsMessengerEXT m_messenger {};
#endif
};
