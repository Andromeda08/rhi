#pragma once

#include "VulkanBase.hpp"
#include "VulkanDebugContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "RHI/DynamicRHI.hpp"

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

    ~VulkanRHI() override = default;

    #pragma region "DynamicRHI"

    std::unique_ptr<RHIBuffer> createBuffer(const RHIBufferCreateInfo& createInfo) override
    {
        return VulkanBuffer::createVulkanBuffer({
            .bufferSize = createInfo.bufferSize,
            .bufferType = createInfo.bufferType,
            .debugName  = createInfo.debugName,
            .pDevice    = mDevice.get(),
        });
    }

    void              waitIdle()         const override { mDevice->waitIdle(); }

    RHICommandQueue*  getGraphicsQueue()       override { return mDevice->getGraphicsQueue(); }
    RHIInterfaceType  getType()          const override { return RHIInterfaceType::Vulkan; }

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

    std::unique_ptr<VulkanDevice>       mDevice;

    std::unique_ptr<VulkanSwapchain>    mSwapchain;

    IRHIWindow*                         mWindow;
};