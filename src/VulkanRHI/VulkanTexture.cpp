#include "VulkanTexture.hpp"

#include "VulkanDevice.hpp"
#include "VulkanAllocator.hpp"

VulkanTexture::VulkanTexture(const VulkanTextureCreateInfo& createInfo)
: RHITexture()
, mSize(toVulkan(createInfo.size))
, mFormat(toVulkan(createInfo.format))
, mDevice(createInfo.pDevice)
, mDebugName(createInfo.debugName)
{
    using enum vk::ImageUsageFlagBits;
    auto usageFlags = eTransferSrc | eTransferDst | eStorage;

    if (isDepthFormat(createInfo.format))
    {
        usageFlags |= eDepthStencilAttachment;
    }
    else
    {
        usageFlags |= eColorAttachment;
    }

    if (createInfo.sampled)
    {
        usageFlags |= eSampled;
    }

    const auto imageCreateInfo = vk::ImageCreateInfo()
        .setFormat(mFormat)
        .setExtent({ mSize.width, mSize.height, 1 })
        .setSamples(vk::SampleCountFlagBits::e1)
        .setUsage(usageFlags)
        .setTiling(vk::ImageTiling::eOptimal)
        .setArrayLayers(1)
        .setMipLevels(1)
        .setImageType(vk::ImageType::e2D)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);

    if (const vk::Result result = mDevice->handle().createImage(&imageCreateInfo, nullptr, &mImage);
        result != vk::Result::eSuccess)
    {
        auto msg = fmt::format("Failed to create Image \"{}\" ({})", mDebugName, to_string(result));
        VK_PRINTLN(msg);
        throw std::runtime_error(msg);
    }

    mDevice->nameObject<vk::Image>({
        .debugName = mDebugName.c_str(),
        .handle = mImage,
    });

    mAllocation = mDevice->allocateMemory({
        .propertyFlags = vk::MemoryPropertyFlagBits::eDeviceLocal,
        .target = mImage
    });
    mAllocation->bind();

    auto viewCreateInfo = vk::ImageViewCreateInfo()
        .setFormat(mFormat)
        .setImage(mImage)
        .setViewType(vk::ImageViewType::e2D);

    if (isDepthFormat(createInfo.format))
    {
        viewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1 });
    }
    else
    {
        viewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
    }

    if (const vk::Result result = mDevice->handle().createImageView(&viewCreateInfo, nullptr, &mImageView);
        result != vk::Result::eSuccess)
    {
        const auto msg = fmt::format("Failed to create ImageView \"{}\" ({})", mDebugName, to_string(result));
        VK_PRINTLN(msg);
        throw std::runtime_error(msg);
    }

    const auto viewName = fmt::format("{} [ImageView]", mDebugName);
    mDevice->nameObject<vk::ImageView>({
        .debugName = viewName.c_str(),
        .handle = mImageView,
    });

    if (createInfo.sampled)
    {
        auto samplerCreateInfo = vk::SamplerCreateInfo()
            .setMagFilter(vk::Filter::eLinear)
            .setMinFilter(vk::Filter::eLinear)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setAnisotropyEnable(false)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
            .setUnnormalizedCoordinates(false)
            .setCompareEnable(false)
            .setCompareOp(vk::CompareOp::eAlways)
            .setMipmapMode(vk::SamplerMipmapMode::eLinear)
            .setMipLodBias(0.0f)
            .setMinLod(0.0f)
            .setMaxLod(0.0f);

        if (const vk::Result result = mDevice->handle().createSampler(&samplerCreateInfo, nullptr, &mSampler);
            result != vk::Result::eSuccess)
        {
            const auto msg = fmt::format("Failed to create Sampler for Image \"{}\" ({})", mDebugName, to_string(result));
            VK_PRINTLN(msg);
            throw std::runtime_error(msg);
        }

        const auto samplerName = fmt::format("{} [Sampler]", mDebugName);
        mDevice->nameObject<vk::Sampler>({
            .debugName = samplerName.c_str(),
            .handle = mSampler,
        });
    }

    VK_VERBOSE(fmt::format("Created Texture (debugName: {})", createInfo.debugName.empty() ? "-" : createInfo.debugName));
}

std::unique_ptr<VulkanTexture> VulkanTexture::createVulkanTexture(const VulkanTextureCreateInfo& createInfo)
{
    return std::make_unique<VulkanTexture>(createInfo);
}

VulkanTexture::~VulkanTexture()
{
    mDevice->handle().destroy(mSampler);
    mDevice->handle().destroy(mImageView);
    mDevice->handle().destroy(mImage);
    mAllocation->free();
}
