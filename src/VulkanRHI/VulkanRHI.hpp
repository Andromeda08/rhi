#pragma once

#include "VulkanCore.hpp"
#include "VulkanLayer.hpp"
#include "VulkanExtension.hpp"

using VulkanDevicePtr = std::shared_ptr<class VulkanDevice>;

class VulkanRHI : public DynamicRHI
{
public:
    DEF_PRIMARY_CTOR(VulkanRHI);

    ~VulkanRHI() override = default;

    void init() override;

    RHIInterfaceType getType() const override { return RHIInterfaceType::Vulkan; }

    vk::Instance getInstance() const { return mInstance; }
    VulkanDevicePtr getDevice() const { return mDevice; }

private:
    void createInstance();

    vk::PhysicalDevice selectPhysicalDevice() const;

    void createDevice();

    void createDebugUtilsMessenger() {}

    vk::Instance mInstance;
    VulkanInstanceLayers mInstanceLayers;
    VulkanInstanceExtensions mInstanceExtensions;

    VulkanDevicePtr mDevice;

    vk::DebugUtilsMessengerEXT mMessenger {};
};
