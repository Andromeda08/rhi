#pragma once

#include "VulkanBase.hpp"

struct VulkanDebugContextCreateInfo
{
    vk::Instance instance;
};

class VulkanDebugContext
{
public:
    DISABLE_COPY_CTOR(VulkanDebugContext);
    explicit DEF_PRIMARY_CTOR(VulkanDebugContext, const VulkanDebugContextCreateInfo& createInfo);

    ~VulkanDebugContext();

    static VkBool32 VKAPI_CALL debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* p_data, void* p_user);

private:
    vk::DebugUtilsMessengerEXT mMessenger;
    vk::Instance               mInstance;
};