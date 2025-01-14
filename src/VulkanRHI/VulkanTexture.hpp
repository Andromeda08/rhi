#pragma once

#include "VulkanBase.hpp"
#include <RHI/RHITexture.hpp>

class VulkanDevice;
class VulkanAllocation;

struct VulkanTextureCreateInfo
{
    Size2D          size;
    Format          format  {Format::R32G32B32A32Sfloat};
    bool            sampled {true};
    std::string     debugName;
    VulkanDevice*   pDevice;
};

class VulkanTexture : public RHITexture
{
public:
    DISABLE_COPY_CTOR(VulkanTexture);
    explicit DEF_PRIMARY_CTOR(VulkanTexture, const VulkanTextureCreateInfo& createInfo);

    ~VulkanTexture() override;

    const vk::Image&        getImage()      const { return mImage; }
    const vk::ImageView&    getImageView()  const { return mImageView; }
    const vk::Sampler&      getSampler()    const { return mSampler; }

private:
    vk::Image           mImage;
    vk::ImageView       mImageView;
    vk::Sampler         mSampler;

    vk::Extent2D        mSize;
    vk::Format          mFormat;

    VulkanAllocation*   mAllocation;
    VulkanDevice*       mDevice;
    std::string         mDebugName;
};