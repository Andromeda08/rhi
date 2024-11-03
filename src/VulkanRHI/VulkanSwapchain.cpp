#include "VulkanSwapchain.hpp"

#include "VulkanDevice.hpp"

VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainParams& params)
: mImageCount(params.imageCount)
, mSurface(params.surface)
, mWindow(params.window)
, mDevice(params.device)
{
    checkSwapchainSupport();
    createSwapchain();
    acquireImages();
    createFrameSyncObjects();
    makeDynamicState();

    mAspectRatio = toRHI(mExtent).aspectRatio();
}

std::shared_ptr<VulkanSwapchain> VulkanSwapchain::createVulkanSwapchain(const VulkanSwapchainParams& params)
{
    return std::make_shared<VulkanSwapchain>(params);
}

uint32_t VulkanSwapchain::getNextFrameIndex(uint32_t currentFrame) const
{
    return -1;
}

void VulkanSwapchain::present() const
{
}

void VulkanSwapchain::checkSwapchainSupport()
{
    const auto physicalDevice = mDevice->physicalDevice();
    vk::SurfaceCapabilitiesKHR surfaceCaps = physicalDevice.getSurfaceCapabilitiesKHR(mSurface);
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(mSurface);
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(mSurface);

    mCurrentTransform = surfaceCaps.currentTransform;

    // Capability Checks
    if (surfaceCaps.minImageCount > mImageCount ||
        surfaceCaps.maxImageCount < mImageCount)
    {
        throw std::runtime_error(fmt::format("Swapchain image count {} out of supported range", mImageCount));
    }

    if (surfaceCaps.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        mExtent = vk::Extent2D()
            .setWidth(surfaceCaps.currentExtent.width)
            .setHeight(surfaceCaps.currentExtent.height);
    }

    vk::Extent2D min = surfaceCaps.minImageExtent;
    vk::Extent2D max = surfaceCaps.maxImageExtent;

    Size2D wsize = mWindow->framebufferSize();
    mExtent.width = std::clamp(wsize.width, min.width, max.width);
    mExtent.height = std::clamp(wsize.height, min.height, max.height);

    // Surface Format & ColorSpace
    if (surfaceFormats.empty())
    {
        throw std::runtime_error("No surface formats found");
    }
    bool formatOkay = false;
    for (const auto& format : surfaceFormats)
    {
        if (format.format == mFormat && format.colorSpace == mColorSpace)
        {
            formatOkay = true;
        }
    }
    if (!formatOkay)
    {
        throw std::runtime_error("Surface Format error");
    }

    // Present Mode
    if (presentModes.empty())
    {
        throw std::runtime_error("No present modes found");
    }
    if (const auto it = std::ranges::find(presentModes, mPresentMode);
        it == std::end(presentModes))
    {
        throw std::runtime_error("PresentMode error");
    }
}

void VulkanSwapchain::createSwapchain()
{
    const auto createInfo = vk::SwapchainCreateInfoKHR()
        .setSurface(mSurface)
        .setMinImageCount(mImageCount)
        .setImageFormat(mFormat)
        .setImageColorSpace(mColorSpace)
        .setImageExtent(mExtent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
        .setPreTransform(mCurrentTransform)
        .setClipped(true)
        .setOldSwapchain(nullptr)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setPresentMode(mPresentMode)
        .setQueueFamilyIndexCount(0)
        .setPQueueFamilyIndices(nullptr);

    mDevice->createSwapchain({
        .createInfo = createInfo,
        .pSwapchain = &mSwapchain
    });
}

void VulkanSwapchain::acquireImages()
{
    mImages.resize(mImageCount);
    mImages = mDevice->handle().getSwapchainImagesKHR(mSwapchain);

    vk::ComponentMapping componentMapping = {
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity
    };

    auto create_info = vk::ImageViewCreateInfo()
        .setComponents(componentMapping)
        .setFormat(mFormat)
        .setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 })
        .setViewType(vk::ImageViewType::e2D);

    mImageViews.resize(mImageCount);
    for (uint32_t i = 0; i < mImageCount; i++)
    {
        create_info.setImage(mImages[i]);
        if (const vk::Result result = mDevice->handle().createImageView(&create_info, nullptr, &mImageViews[i]);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error(fmt::format("Failed to create vk::ImageView #{} for Swapchain", i));
        }

        mDevice->nameObject(mImages[i], fmt::format("Swapchain #{}", i), vk::ObjectType::eImage);
        mDevice->nameObject(mImageViews[i], fmt::format("Swapchain ImageView #{}", i), vk::ObjectType::eImageView);
    }
}

void VulkanSwapchain::createFrameSyncObjects()
{
    mImageReady.resize(mImageCount);
    mRenderingFinished.resize(mImageCount);
    mFrameInFLight.resize(mImageCount);

    for (size_t i = 0; i < mImageCount; i++)
    {
        auto sci = vk::SemaphoreCreateInfo();
        auto fci = vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled);

        vk::Result result = mDevice->handle().createSemaphore(&sci, nullptr, &mImageReady[i]);
        mDevice->nameObject(mImageReady[i], fmt::format("ImageReady {}", i), vk::ObjectType::eSemaphore);

        result = mDevice->handle().createSemaphore(&sci, nullptr, &mRenderingFinished[i]);
        mDevice->nameObject(mRenderingFinished[i], fmt::format("RenderingFinished {}", i), vk::ObjectType::eSemaphore);

        result = mDevice->handle().createFence(&fci, nullptr, &mFrameInFLight[i]);
        mDevice->nameObject(mFrameInFLight[i], fmt::format("FrameInFlight {}", i), vk::ObjectType::eFence);
    }
}

void VulkanSwapchain::makeDynamicState()
{
    mCachedScissor = vk::Rect2D {{ 0, 0 }, mExtent};

    /**
     * Create a viewport object based on the current state of the Swapchain.
     * The viewport is flipped along the Y axis for GLM compatibility.
     * This requires Maintenance1, which is core Vulkan since API version 1.1.
     * https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/
     */
    mCachedViewport = vk::Viewport()
        .setX(0.0f)
        .setWidth(static_cast<float>(mExtent.width))
        .setY(static_cast<float>(mExtent.height))
        .setHeight(-1.0f * static_cast<float>(mExtent.height))
        .setMaxDepth(1.0f)
        .setMinDepth(0.0f);
}
