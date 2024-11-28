#pragma once

#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/Framebuffer.hpp"

struct VulkanFramebuffer final : public RHIFramebuffer
{
    explicit VulkanFramebuffer(const vk::Framebuffer framebuffer): RHIFramebuffer(), mFramebuffer(framebuffer) {}

    ~VulkanFramebuffer() override = default;

    vk::Framebuffer handle() const { return mFramebuffer; }

private:
    vk::Framebuffer mFramebuffer;
};

struct VulkanFramebuffersInfo
{
    VulkanFramebuffersInfo& addAttachment(
        const vk::ImageView& imageView,
        std::optional<uint32_t> attachmentIndex = std::nullopt,
        std::optional<uint32_t> framebufferIndex = std::nullopt)
    {
        if (framebufferCount <= 0)
        {
            throw std::runtime_error("No Framebuffer count was specified");
        }

        // Decide on attachment index
        uint32_t attachment_index = std::max(lastAttachmentIndex + 1, 0); // Default on the next index
        if (attachmentIndex.has_value())
        {
            auto value = attachmentIndex.value();
            // 1. Already existing location
            if (value <= lastAttachmentIndex)
            {
                attachment_index = value;
            }
        }

        // Decide on framebuffer range
        uint32_t first = 0;
        uint32_t last  = framebufferCount - 1;
        if (framebufferIndex.has_value())
        {
            first = last = framebufferIndex.value();
        }

        // Set attachment for framebuffers
        for (uint32_t i = first; i < last + 1; i++)
        {
            // New fb_idx -> Insert empty vector into map
            if (!attachments.contains(i))
            {
                attachments.insert({i, {}});
            }

            auto& vec = attachments[i];
            if (attachment_index <= vec.size())
            {
                vec.resize(attachment_index + 1);
            }

            vec[attachment_index] = imageView;
        }

        lastAttachmentIndex = static_cast<int32_t>(attachment_index);
        return *this;
    }

    VulkanFramebuffersInfo& setCount(uint32_t value)
    {
        framebufferCount = value;
        return *this;
    }

    VulkanFramebuffersInfo& validate()
    {
        {
            bool has_missing_attachments = false;
            for (auto& vec: attachments | std::views::values)
            {
                if (!std::ranges::all_of(vec, [&](const auto& b){ return b; }))
                {
                    has_missing_attachments = true;
                }
            }

            if (has_missing_attachments)
            {
                for (auto& [id, vec] : attachments)
                {
                    throw std::runtime_error("Missing attachments for Framebuffers");
                }
            }

            if (extent.width == 0 || extent.height == 0)
            {
                throw std::runtime_error("Invalid Extent for Framebuffers");
            }

            return *this;
        }
    }

    uint32_t        framebufferCount {0};
    vk::RenderPass  renderPass;
    vk::Extent2D    extent;
    VulkanDevice*   device;
    const char*     debugName;

    std::map<uint32_t, std::vector<vk::ImageView>> attachments {};
    int32_t lastAttachmentIndex {-1};

    friend class VulkanFramebuffers;
};

class VulkanFramebuffers final : public RHIFramebuffers
{
public:
    DISABLE_COPY_CTOR(VulkanFramebuffers);
    explicit DEF_PRIMARY_CTOR(VulkanFramebuffers, VulkanFramebuffersInfo& framebuffersInfo);

    ~VulkanFramebuffers() override = default;

    RHIFramebuffer* getFramebuffer(const size_t index) override
    {
        if (index >= mFramebuffers.size())
        {
            throw std::runtime_error("Index out of range");
        }

        return mFramebuffers[index].get();
    }

private:
    std::vector<std::unique_ptr<VulkanFramebuffer>> mFramebuffers;
    VulkanDevice* mDevice;
};

inline VulkanFramebuffers::VulkanFramebuffers(VulkanFramebuffersInfo& framebuffersInfo)
: RHIFramebuffers()
,mDevice(framebuffersInfo.device)
{
    auto fb_create_info = vk::FramebufferCreateInfo()
        .setAttachmentCount(framebuffersInfo.lastAttachmentIndex + 1)
        .setHeight(framebuffersInfo.extent.height)
        .setLayers(1)
        .setRenderPass(framebuffersInfo.renderPass)
        .setWidth(framebuffersInfo.extent.width);

    mFramebuffers.resize(framebuffersInfo.framebufferCount);

    std::vector<vk::Framebuffer> framebuffers(framebuffersInfo.framebufferCount);
    for (uint32_t i = 0; i < mFramebuffers.size(); i++)
    {
        fb_create_info.setPAttachments(framebuffersInfo.attachments[i].data());
        if (vk::Result result = mDevice->handle().createFramebuffer(&fb_create_info, nullptr, &framebuffers[i]);
            result != vk::Result::eSuccess)
        {
            throw std::runtime_error("Failed to create Framebuffer");
        }
        mDevice->nameObject<vk::Framebuffer>({
            .debugName = fmt::format("{} #{}", framebuffersInfo.debugName, i).c_str(),
            .handle = framebuffers[i],
        });

        mFramebuffers[i] = std::make_unique<VulkanFramebuffer>(framebuffers[i]);
    }
}

inline std::unique_ptr<VulkanFramebuffers> VulkanFramebuffers::createVulkanFramebuffers(VulkanFramebuffersInfo& framebuffersInfo)
{
    return std::make_unique<VulkanFramebuffers>(framebuffersInfo);
}
