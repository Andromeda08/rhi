#pragma once

#include <memory>

#ifdef VULKAN_RHI_ENABLED
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vk_enum_string_helper.h>
#endif

#include <GLFW/glfw3.h>

#ifdef D3D12_RHI_ENABLED
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#endif

#include "RHI/RHIWindow.hpp"
#include "RHI/Macros.hpp"

struct WindowCreateInfo
{
    Size2D      resolution;
    std::string title;
};

class Window : public RHIWindow
{
public:
    DISABLE_COPY_CTOR(Window);
    explicit DEF_PRIMARY_CTOR(Window, const WindowCreateInfo& createInfo);

    ~Window() override;

    Size2D windowSize() const override;

    Size2D framebufferSize() const override;

    bool shouldClose() const { return glfwWindowShouldClose(mWindow); }

#ifdef D3D12_RHI_ENABLED
    HWND getWin32Handle() const override { return glfwGetWin32Window(mWindow); }
#endif

#ifdef VULKAN_RHI_ENABLED
    void createVulkanSurface(const vk::Instance& instance, vk::SurfaceKHR* pSurface) override;

    std::vector<const char*> getVulkanInstanceExtensions() override;
#endif

private:
    static void exitKeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods);

private:
    GLFWwindow* mWindow {nullptr};
};