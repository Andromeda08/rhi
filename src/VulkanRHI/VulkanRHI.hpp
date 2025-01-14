#pragma once

#include "VulkanBase.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanDebugContext.hpp"
#include "VulkanDevice.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanSwapchain.hpp"
#include "RHI/DynamicRHI.hpp"

struct VulkanRHICreateInfo
{
    RHIWindow* pWindow         = nullptr;
    uint32_t   backBufferCount = 2;
};

class VulkanRHI final : public DynamicRHI
{
public:
    DISABLE_COPY_CTOR(VulkanRHI);
    explicit DEF_PRIMARY_CTOR(VulkanRHI, const VulkanRHICreateInfo& createInfo);

    ~VulkanRHI() override = default;

    #pragma region "DynamicRHI"

    Frame beginFrame(const RHIFrameBeginInfo& frameBeginInfo) override;

    void submitFrame(const Frame& frame) override;


    std::unique_ptr<RHIBuffer> createBuffer(const RHIBufferCreateInfo& createInfo) override;

    std::unique_ptr<RHITexture> createTexture(const RHITextureCreateInfo& createInfo) override;

    std::unique_ptr<RHIRenderPass> createRenderPass(const RHIRenderPassCreateInfo& createInfo) override;

    std::unique_ptr<RHIFramebuffer> createFramebuffer(const RHIFramebufferCreateInfo& createInfo) override;

    std::unique_ptr<RHIPipeline> createPipeline(const RHIPipelineCreateInfo& createInfo) override;


    void              waitIdle()               override { mDevice->waitIdle(); }

    RHICommandQueue*  getGraphicsQueue()       override { return mDevice->getGraphicsQueue(); }
    RHIInterfaceType  getType()          const override { return RHIInterfaceType::Vulkan; }
    RHISwapchain*     getSwapchain()     const override { return mSwapchain.get(); }

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

    RHIWindow*                          mWindow;

    uint32_t                            mFramesInFlight {2};
    uint32_t                            mCurrentFrame {0};

    std::vector<vk::Fence>              mFrameInFlight;
    std::vector<vk::Semaphore>          mImageReady;
    std::vector<vk::Semaphore>          mRenderingFinished;
};