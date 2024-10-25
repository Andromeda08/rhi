#pragma once

#include "VulkanCore.hpp"
#include "RHI/RHISwapchain.hpp"
#include "RHI/IWindow.hpp"

struct VulkanSwapchainParams
{
    std::shared_ptr<IRHIWindow>   window;
    std::shared_ptr<VulkanDevice> device;
    vk::SurfaceKHR                surface;
    uint32_t                      imageCount {2};
};

class VulkanSwapchain : public RHISwapchain
{
public:
    DISABLE_COPY_CTOR(VulkanSwapchain);
    explicit DEF_PRIMARY_CTOR(VulkanSwapchain, const VulkanSwapchainParams& params);

    ~VulkanSwapchain() override = default;

    uint32_t getNextFrameIndex(uint32_t currentFrame) const override;

    void present() const override;

    Rect2D   getScissor()           override { return mCachedScissor; }
    Viewport getViewport()          override { return mCachedViewport; }
    Size2D   getSize()        const override { return mExtent; }
    float    getAspectRatio() const override { return mAspectRatio; }
    Format   getFormat()      const override { return mFormat; }
    uint32_t getFrameCount()        override { return mImageCount; }

private:
    void checkSwapchainSupport();
    void createSwapchain();
    void acquireImages();
    void createFrameSyncObjects();
    void makeDynamicState();

    uint32_t                        mImageCount;
    Size2D                          mExtent;
    float                           mAspectRatio {};
    Format                          mFormat {Format::B8G8R8A8Unorm};
    ColorSpace                      mColorSpace {ColorSpace::SrgbNonLinear};
    vk::PresentModeKHR              mPresentMode {vk::PresentModeKHR::eFifo};
    vk::SurfaceTransformFlagBitsKHR mCurrentTransform;
    vk::SurfaceKHR                  mSurface;
    vk::SwapchainKHR                mSwapchain;
    Rect2D                          mCachedScissor;
    Viewport                        mCachedViewport;

    std::vector<vk::Image>          mImages;
    std::vector<vk::ImageView>      mImageViews;

    std::shared_ptr<IRHIWindow>     mWindow;
    std::shared_ptr<VulkanDevice>   mDevice;

    // Frame Sync
    std::vector<vk::Semaphore>      mImageReady;
    std::vector<vk::Semaphore>      mRenderingFinished;
    std::vector<vk::Fence>          mFrameInFLight;
};