#pragma once

#include <memory>

#ifdef VULKAN_RHI_ENABLED
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#ifdef D3D12_RHI_ENABLED
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

#include "RHI/IWindow.hpp"

struct WindowCreateInfo
{
    Size2D      resolution;
    std::string title;
};

using WindowPtr = std::shared_ptr<class Window>;

class Window : IRHIWindow
{
public:
    explicit DEF_PRIMARY_CTOR(Window, const WindowCreateInfo&);

    ~Window() override;

    Size2D windowSize() const override;

    Size2D framebufferSize() const override;

#ifdef D3D12_RHI_ENABLED
    HWND getWin32Handle() const override { return glfwGetWin32Window(mWindow); }
#endif

#ifdef VULKAN_RHI_ENABLED
    void createVulkanSurface(const vk::Instance& instance, vk::SurfaceKHR* pSurface) override;

    VulkanInstanceExtensions getVulkanInstanceExtensions() override;
#endif

private:
    static void exitKeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

    GLFWwindow* mWindow {nullptr};
};