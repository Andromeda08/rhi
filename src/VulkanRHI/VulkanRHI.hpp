#pragma once

#include <vulkan/vulkan.hpp>

#include "Core/Base.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanLayer.hpp"
#include "VulkanExtension.hpp"

// TODO: Change to fwd decl.
class VulkanDevice {};

class VulkanRHI : public DynamicRHI
{
public:
    VulkanRHI();

    void init() override;

    RHIInterfaceType getType() const override { return RHIInterfaceType::Vulkan; }

    static std::shared_ptr<DynamicRHI> createVulkanRHI();

private:
    void createInstance();
    vk::PhysicalDevice selectPhysicalDevice() {}
    void createDevice() {}

    vk::Instance mInstance;
    VulkanInstanceLayers mInstanceLayers;
    VulkanInstanceExtensions mInstanceExtensions;

    std::shared_ptr<VulkanDevice> m_device;

#if VULKAN_DEBUGGING_ENABLED
    vk::DebugUtilsMessengerEXT mMessenger {};
#endif
};
