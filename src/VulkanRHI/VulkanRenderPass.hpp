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
        vk::ImageLayout         final_layout = vk::ImageLayout::eColorAttachmentOptimal,
        vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1,
        vk::ClearColorValue     clear_value  = {0.0f, 0.0f, 0.0f, 1.0f},
        vk::AttachmentLoadOp    load_op      = vk::AttachmentLoadOp::eClear)
    {
        const auto ad = vk::AttachmentDescription()
            .setFormat(format)
            .setSamples(sample_count)
            .setLoadOp(load_op)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(final_layout);
        attachments.push_back(ad);

        auto ref = vk::AttachmentReference()
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(attachments.size() - 1);
        colorRefs.push_back(ref);

        auto cv = vk::ClearValue(clear_value);
        clearValues.push_back(cv);

        return *this;
    }
};

class VulkanRenderPass final : public RHIRenderPass
{
public:
    DISABLE_COPY_CTOR(VulkanRenderPass);
    explicit DEF_PRIMARY_CTOR(VulkanRenderPass, const VulkanRenderPassInfo& renderPassInfo);

    void execute(RHICommandList* commandList, RHIFramebuffer* framebuffer, std::function<void(RHICommandList*)> lambda) override;

    vk::RenderPass handle() const { return mRenderPass; }

private:
    vk::Rect2D                  mRenderArea;
    vk::RenderPass              mRenderPass;
    vk::RenderPassBeginInfo     mRenderPassBeginInfo;
    std::vector<vk::ClearValue> mClearValues;
    VulkanDevice*               mDevice;
};

inline VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassInfo& renderPassInfo)
: RHIRenderPass()
, mDevice(renderPassInfo.device)
, mRenderArea(renderPassInfo.renderArea)
, mClearValues(renderPassInfo.clearValues)
{
    auto subpass = vk::SubpassDescription()
            .setColorAttachmentCount(static_cast<uint32_t>(renderPassInfo.colorRefs.size()))
            .setInputAttachmentCount(0)
            .setPInputAttachments(nullptr)
            .setPResolveAttachments(renderPassInfo.hasResolveAttachment ? &renderPassInfo.resolveRef : nullptr)
            .setPColorAttachments(renderPassInfo.colorRefs.data())
            .setPDepthStencilAttachment(renderPassInfo.hasDepthAttachment ? &renderPassInfo.depthRef : nullptr)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

        auto subpass_dependency = vk::SubpassDependency()
            .setSrcSubpass(VK_SUBPASS_EXTERNAL)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setSrcAccessMask({})
            .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
            .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);


        auto rp_renderPassInfo = vk::RenderPassCreateInfo()
            .setAttachmentCount(renderPassInfo.attachments.size())
            .setPAttachments(renderPassInfo.attachments.data())
            .setSubpassCount(1)
            .setPSubpasses(&subpass)
            .setDependencyCount(1)
            .setPDependencies(&subpass_dependency);

        if (const vk::Result result = mDevice->handle().createRenderPass(&rp_renderPassInfo, nullptr, &mRenderPass);
            result != vk::Result::eSuccess)
        {
            // TODO
            throw std::runtime_error("big bad");
        }

        mDevice->nameObject<vk::RenderPass>({
            .debugName = renderPassInfo.debugName,
            .handle = mRenderPass,
        });

        mRenderPassBeginInfo = vk::RenderPassBeginInfo()
            .setRenderArea(mRenderArea)
            .setRenderPass(mRenderPass)
            .setClearValueCount(mClearValues.size())
            .setPClearValues(mClearValues.data());
}

inline std::unique_ptr<VulkanRenderPass> VulkanRenderPass::createVulkanRenderPass(const VulkanRenderPassInfo& renderPassInfo)
{
    return std::make_unique<VulkanRenderPass>(renderPassInfo);
}

inline void VulkanRenderPass::execute(RHICommandList* commandList, RHIFramebuffer* framebuffer,
                                      const std::function<void(RHICommandList*)> lambda)
{
    const auto commandBuffer = commandList->as< VulkanCommandList>()->handle();

    mRenderPassBeginInfo.setFramebuffer(framebuffer->as<VulkanFramebuffer>()->handle());
    commandBuffer.beginRenderPass(&mRenderPassBeginInfo, vk::SubpassContents::eInline);

    lambda(commandList);

    commandBuffer.endRenderPass();
}
