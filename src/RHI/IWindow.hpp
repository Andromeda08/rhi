#pragma once

#include "RHIDefinitions.hpp"

#ifdef D3D12_RHI_ENABLED
#include <windef.h>
#endif

#ifdef VULKAN_RHI_ENABLED
#include <vulkan/vulkan.hpp>
#include "VulkanRHI/VulkanExtension.hpp"
#endif

class IRHIWindow
{
public:
    IRHIWindow() = default;

    virtual ~IRHIWindow() = default;

    virtual Size2D windowSize() const = 0;
    virtual Size2D framebufferSize() const = 0;

#ifdef VULKAN_RHI_ENABLED
    virtual void createVulkanSurface(const vk::Instance& instance, vk::SurfaceKHR* pSurface) = 0;
    virtual VulkanInstanceExtensions getVulkanInstanceExtensions() = 0;
#endif

#ifdef D3D12_RHI_ENABLED
    virtual HWND getWin32Handle() const = 0;
#endif
};