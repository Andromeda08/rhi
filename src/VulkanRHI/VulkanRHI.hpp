#pragma once

#include "VulkanCore.hpp"
#include "VulkanLayer.hpp"
#include "VulkanExtension.hpp"
#include "VulkanCommandQueue.hpp"

class VulkanRHI : public DynamicRHI
{
public:
    DEF_PRIMARY_CTOR(VulkanRHI);

    ~VulkanRHI() override = default;

    void init() override;

    std::shared_ptr<RHICommandQueue> getGraphicsQueue() override { return nullptr; }

    RHIInterfaceType getType() const override { return RHIInterfaceType::Vulkan; }

    vk::Instance getInstance() const { return mInstance; }
    std::shared_ptr<VulkanDevice> getDevice() const { return mDevice; }

private:
    void createInstance();

    vk::PhysicalDevice selectPhysicalDevice() const;

    void createDevice();

    vk::Instance mInstance;
    VulkanInstanceLayers mInstanceLayers;
    VulkanInstanceExtensions mInstanceExtensions;

    std::shared_ptr<VulkanDevice> mDevice;

    vk::DebugUtilsMessengerEXT mMessenger {};
};
