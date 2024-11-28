#pragma once

#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/RHISwapchain.hpp"
#include "RHI/RHIWindow.hpp"

struct VulkanSwapchainCreateInfo
{
    RHIWindow*    pWindow = nullptr;
    VulkanDevice*  pDevice = nullptr;
    vk::Instance   instance;
    uint32_t       imageCount {};
};

class VulkanSwapchain final : public RHISwapchain
{
public:
    DISABLE_COPY_CTOR(VulkanSwapchain);
    explicit DEF_PRIMARY_CTOR(VulkanSwapchain, const VulkanSwapchainCreateInfo& params);

    ~VulkanSwapchain() override;

    uint32_t getNextFrameIndex(uint32_t currentFrame) const override;

    void present(vk::Semaphore waitSemaphore, uint32_t imageIndex) const;

    void setScissorViewport(RHICommandList* commandList) const override;

    vk::SwapchainKHR handle() const { return mSwapchain; }

    Rect2D   getScissor()           override { return toRHI(mCachedScissor); }
    Viewport getViewport()          override { return toRHI(mCachedViewport); }
    Size2D   getSize()        const override { return toRHI(mExtent); }
    float    getAspectRatio() const override { return mAspectRatio; }
    Format   getFormat()      const override { return toRHI(mFormat); }
    uint32_t getFrameCount()        override { return mImageCount; }


    vk::Extent2D getExtent()   const { return mExtent; }
    vk::Format   getFormatVk() const { return mFormat; }

    vk::ImageView getImageView(size_t i) const;

private:
    void createSurface();
    void checkSwapchainSupport();
    void createSwapchain();
    void acquireImages();
    void makeDynamicState();

private:
    const uint32_t                  mImageCount;
    vk::Extent2D                    mExtent;
    float                           mAspectRatio;
    vk::Format                      mFormat {vk::Format::eB8G8R8A8Unorm};
    vk::ColorSpaceKHR               mColorSpace {vk::ColorSpaceKHR::eSrgbNonlinear};
    vk::PresentModeKHR              mPresentMode {vk::PresentModeKHR::eFifo};
    vk::SurfaceTransformFlagBitsKHR mCurrentTransform {};
    vk::SurfaceKHR                  mSurface;
    vk::SwapchainKHR                mSwapchain;
    vk::Rect2D                      mCachedScissor;
    vk::Viewport                    mCachedViewport;

    std::vector<vk::Image>          mImages;
    std::vector<vk::ImageView>      mImageViews;

    RHIWindow*                      mWindow;
    VulkanDevice*                   mDevice;
    vk::Instance                    mInstance;
};