#pragma once

#include <vulkan/vulkan.hpp>

class VulkanPlatform
{
public:
    static constexpr uint32_t getPlatformVulkanFeatureLevel() noexcept
    {
    #ifdef __APPLE__
        return VK_API_VERSION_1_2;
    #endif

        return VK_API_VERSION_1_3;
    }
};