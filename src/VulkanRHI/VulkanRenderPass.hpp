#pragma once

#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "VulkanFramebuffer.hpp"
#include "RHI/RHIRenderPass.hpp"

struct VulkanRenderPassInfo
{
    std::vector<vk::ClearValue>            clearValues {};
    std::vector<vk::AttachmentDescription> attachments {};
    std::vector<vk::AttachmentReference>   colorRefs {};
    vk::AttachmentReference                depthRef {};
    bool                                   hasDepthAttachment {false};
    vk::AttachmentReference                resolveRef {};
    bool                                   hasResolveAttachment {false};
    vk::Rect2D                             renderArea;
    VulkanDevice*                          device;
    const char*                            debugName;

    VulkanRenderPassInfo& addColorAttachment(
        vk::Format              format,
        vk::ImageLayout         finalLayout = vk::ImageLayout::eColorAttachmentOptimal,
        vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1,
        vk::ClearColorValue     clearValue  = {0.0f, 0.0f, 0.0f, 1.0f},
        vk::AttachmentLoadOp    loadOp      = vk::AttachmentLoadOp::eClear);
};

class VulkanRenderPass final : public RHIRenderPass
{
public:
    DISABLE_COPY_CTOR(VulkanRenderPass);
    explicit DEF_PRIMARY_CTOR(VulkanRenderPass, const VulkanRenderPassInfo& renderPassInfo);

    void execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer, std::function<void(RHICommandList*)> lambda) override;

    vk::RenderPass handle() const { return mRenderPass; }

private:
    vk::Rect2D                  mRenderArea;
    vk::RenderPass              mRenderPass;
    vk::RenderPassBeginInfo     mRenderPassBeginInfo;
    std::vector<vk::ClearValue> mClearValues;
    VulkanDevice*               mDevice;
};
