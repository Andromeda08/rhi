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
        mExtent = Size2D()
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
        if (format.format == toVulkan(mFormat) &&
            format.colorSpace == toVulkan(mColorSpace))
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
        .setImageFormat(toVulkan(mFormat))
        .setImageColorSpace(toVulkan(mColorSpace))
        .setImageExtent(toVulkan(mExtent))
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
        .setPreTransform(mCurrentTransform)
        .setClipped(true)
        .setOldSwapchain(nullptr)
        .setImageSharingMode(vk::SharingMode::eExclusive)
        .setPresentMode(mPresentMode)
        .setQueueFamilyIndexCount(0)
        .setPQueueFamilyIndices(nullptr);

    if (const vk::Result result = mDevice->handle().createSwapchainKHR(&createInfo, nullptr, &mSwapchain);
        result != vk::Result::eSuccess)
    {
        throw std::runtime_error(fmt::format("Failed to create Swapchain ({})", to_string(result)));
    }
}

void VulkanSwapchain::acquireImages()
{
}

void VulkanSwapchain::createFrameSyncObjects()
{
}

void VulkanSwapchain::makeDynamicState()
{
}
