#pragma once

#include "RHI/DynamicRHI.hpp"
#include "VulkanCore.hpp"
#include "VulkanLayer.hpp"
#include "VulkanExtension.hpp"
#include "VulkanCommandQueue.hpp"

class VulkanRHI : public DynamicRHI
{
public:
    DEF_PRIMARY_CTOR(VulkanRHI);

    ~VulkanRHI() override = default;

    void init(const std::shared_ptr<IRHIWindow>& window) override;

    std::shared_ptr<RHICommandQueue> getGraphicsQueue() override { return nullptr; }

    RHIInterfaceType getType() const override { return RHIInterfaceType::Vulkan; }

    vk::Instance getInstance() const { return mInstance; }
    std::shared_ptr<VulkanDevice> getDevice() const { return mDevice; }

private:
    void createInstance();

    vk::PhysicalDevice selectPhysicalDevice() const;

    void createDevice();

    void createSurface(const std::shared_ptr<IRHIWindow>& window);

    vk::Instance mInstance;
    VulkanInstanceLayers mInstanceLayers;
    VulkanInstanceExtensions mInstanceExtensions;

    std::shared_ptr<VulkanDevice> mDevice;

    vk::SurfaceKHR mSurface;
    std::shared_ptr<VulkanSwapchain> mSwapchain;
    std::shared_ptr<IRHIWindow> mWindow;

    vk::DebugUtilsMessengerEXT mMessenger {};
    void initializeDebugFeatures();

    static VkBool32 VKAPI_CALL debugMessageCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* p_data, void* p_user);
};
