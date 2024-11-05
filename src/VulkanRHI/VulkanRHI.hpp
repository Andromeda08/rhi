#pragma once

#include "VulkanBase.hpp"
#include "RHI/DynamicRHI.hpp"

#include "VulkanDebugContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"

struct VulkanRHICreateInfo
{
    IRHIWindow* pWindow         = nullptr;
    uint32_t    backBufferCount = 2;
};

class VulkanRHI final : public DynamicRHI
{
public:
    DISABLE_COPY_CTOR(VulkanRHI);
    explicit DEF_PRIMARY_CTOR(VulkanRHI, const VulkanRHICreateInfo& createInfo);

    ~VulkanRHI() override
    {
        waitIdle();
        fmt::println("VulkanRHI DTOR");
    };

    #pragma region "DynamicRHI"

    void                             waitIdle()         const override { mDevice->waitIdle(); }

    std::shared_ptr<RHICommandQueue> getGraphicsQueue()       override { return nullptr; }
    RHIInterfaceType                 getType()          const override { return RHIInterfaceType::Vulkan; }

    #pragma endregion

private:
    void createInstance();

    static std::vector<const char*> getSupportedInstanceLayers(const std::vector<const char*>& additionalLayers = {});
    static std::vector<const char*> getSupportedInstanceExtensions(const std::vector<const char*>& additionalExtensions = {});

    void createDebugContext();

    void createDevice();

private:
    vk::Instance                        mInstance;
    std::vector<const char*>            mInstanceLayers;
    std::vector<const char*>            mInstanceExtensions;

    std::unique_ptr<VulkanDebugContext> mDebugContext;

    std::shared_ptr<VulkanDevice>       mDevice;

    std::unique_ptr<VulkanSwapchain>    mSwapchain;

    IRHIWindow*                         mWindow;
};