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
}

void VulkanSwapchain::createSwapchain()
{
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
