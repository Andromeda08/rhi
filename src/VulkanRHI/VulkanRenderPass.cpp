#include "VulkanRenderPass.hpp"

VulkanRenderPassInfo& VulkanRenderPassInfo::addColorAttachment(const vk::Format format, const vk::ImageLayout finalLayout,
    const vk::SampleCountFlagBits sampleCount, const vk::ClearColorValue clearValue, const vk::AttachmentLoadOp loadOp)
{
    const auto ad = vk::AttachmentDescription()
            .setFormat(format)
            .setSamples(sampleCount)
            .setLoadOp(loadOp)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(finalLayout);

    attachments.push_back(ad);

    const auto ref = vk::AttachmentReference()
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal)
            .setAttachment(attachments.size() - 1);
    colorRefs.push_back(ref);

    const auto cv = vk::ClearValue(clearValue);
    clearValues.push_back(cv);

    return *this;
}

VulkanRenderPass::VulkanRenderPass(const VulkanRenderPassInfo& renderPassInfo)
: RHIRenderPass()
, mRenderArea(renderPassInfo.renderArea)
, mClearValues(renderPassInfo.clearValues)
, mDevice(renderPassInfo.device)
{
    const auto subpass = vk::SubpassDescription()
            .setColorAttachmentCount(static_cast<uint32_t>(renderPassInfo.colorRefs.size()))
            .setInputAttachmentCount(0)
            .setPInputAttachments(nullptr)
            .setPResolveAttachments(renderPassInfo.hasResolveAttachment ? &renderPassInfo.resolveRef : nullptr)
            .setPColorAttachments(renderPassInfo.colorRefs.data())
            .setPDepthStencilAttachment(renderPassInfo.hasDepthAttachment ? &renderPassInfo.depthRef : nullptr)
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);

    const auto subpass_dependency = vk::SubpassDependency()
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)
        .setDstSubpass(0)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setSrcAccessMask({})
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);


    const auto rp_renderPassInfo = vk::RenderPassCreateInfo()
        .setAttachmentCount(renderPassInfo.attachments.size())
        .setPAttachments(renderPassInfo.attachments.data())
        .setSubpassCount(1)
        .setPSubpasses(&subpass)
        .setDependencyCount(1)
        .setPDependencies(&subpass_dependency);

    if (const vk::Result result = mDevice->handle().createRenderPass(&rp_renderPassInfo, nullptr, &mRenderPass);
        result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create RenderPass");
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

std::unique_ptr<VulkanRenderPass> VulkanRenderPass::createVulkanRenderPass(const VulkanRenderPassInfo& renderPassInfo)
{
    return std::make_unique<VulkanRenderPass>(renderPassInfo);
}

void VulkanRenderPass::execute(RHICommandList* commandList, RHIFramebufferHandle* framebuffer,
    const std::function<void(RHICommandList*)> lambda)
{
    const auto commandBuffer = commandList->as< VulkanCommandList>()->handle();

    mRenderPassBeginInfo.setFramebuffer(framebuffer->as<VulkanFramebufferHandle>()->handle());
    commandBuffer.beginRenderPass(&mRenderPassBeginInfo, vk::SubpassContents::eInline);

    lambda(commandList);

    commandBuffer.endRenderPass();
}
