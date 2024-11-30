#pragma once

#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "RHI/RHIFramebuffer.hpp"

class VulkanFramebufferHandle final : public RHIFramebufferHandle
{
public:
    explicit VulkanFramebufferHandle(vk::Framebuffer framebuffer);

    ~VulkanFramebufferHandle() override = default;

    vk::Framebuffer handle() const { return mFramebuffer; }

private:
    vk::Framebuffer mFramebuffer;
};

struct VulkanFramebufferInfo
{
    VulkanFramebufferInfo& addAttachment(const vk::ImageView& imageView,
        std::optional<uint32_t> attachmentIndex = std::nullopt,
        std::optional<uint32_t> framebufferIndex = std::nullopt);

    VulkanFramebufferInfo& setCount(uint32_t value) noexcept;

    VulkanFramebufferInfo& validate();

    uint32_t        framebufferCount {0};
    vk::RenderPass  renderPass;
    vk::Extent2D    extent;
    VulkanDevice*   device;
    const char*     debugName;

    std::map<uint32_t, std::vector<vk::ImageView>> attachments {};
    int32_t lastAttachmentIndex {-1};
};

class VulkanFramebuffer final : public RHIFramebuffer
{
public:
    DISABLE_COPY_CTOR(VulkanFramebuffer);
    explicit DEF_PRIMARY_CTOR(VulkanFramebuffer, VulkanFramebufferInfo& framebuffersInfo);

    ~VulkanFramebuffer() override;

    RHIFramebufferHandle* getFramebuffer(size_t index) override;

private:
    std::vector<std::unique_ptr<VulkanFramebufferHandle>> mFramebuffers;
    VulkanDevice* mDevice;
};
