#include "Window.hpp"

Window::Window(const WindowCreateInfo& createInfo)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, false);

    mWindow = glfwCreateWindow(createInfo.resolution.width, createInfo.resolution.height, createInfo.title.c_str(), nullptr, nullptr);

    if (!mWindow)
    {
        throw std::runtime_error("Failed to create Window");
    }

    glfwSetKeyCallback(mWindow, Window::exitKeyHandler);
}

WindowPtr Window::createWindow(const WindowCreateInfo& createInfo)
{
    return std::make_shared<Window>(createInfo);
}

Window::~Window()
{
    if (mWindow)
    {
        glfwDestroyWindow(mWindow);
    }
    glfwTerminate();
}

Size2D Window::windowSize() const
{
    int width, height;
    glfwGetWindowSize(mWindow, &width, &height);
    return {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
}

Size2D Window::framebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(mWindow, &width, &height);
    return {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };
}

void Window::exitKeyHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

#ifdef VULKAN_RHI_ENABLED

void Window::createVulkanSurface(const vk::Instance& instance, vk::SurfaceKHR* pSurface)
{
    if (const VkResult result = glfwCreateWindowSurface(instance, mWindow, nullptr, reinterpret_cast<VkSurfaceKHR*>(pSurface));
        result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Surface for Window");
    }
}

VulkanInstanceExtensions Window::getVulkanInstanceExtensions()
{
    uint32_t extension_count = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extension_count);
    const auto extensionNames = std::vector<const char*>(glfwExtensions, glfwExtensions + extension_count);

    VulkanInstanceExtensions extensions;

    for (const auto& name : extensionNames)
    {
        extensions.push_back(std::make_unique<VulkanInstanceExtension>(name, true));
    }

    return extensions;
}

#endif
