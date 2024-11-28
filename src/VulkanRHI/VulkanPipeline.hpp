#pragma once

#include "RHI/RHIPipeline.hpp"
#include "VulkanBase.hpp"
#include "VulkanDevice.hpp"
#include "VulkanRenderPass.hpp"
#include "RHI/RHIRenderPass.hpp"

struct VulkanPipelineUtils
{
    static vk::PipelineInputAssemblyStateCreateInfo makeInputAssemblyState()
    {
        return vk::PipelineInputAssemblyStateCreateInfo()
            .setTopology(vk::PrimitiveTopology::eTriangleList)
            .setPrimitiveRestartEnable(false)
            .setFlags({})
            .setPNext(nullptr);
    }

    static vk::PipelineRasterizationStateCreateInfo makeRasterizationState()
    {
        return vk::PipelineRasterizationStateCreateInfo()
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eBack)
            .setFrontFace(vk::FrontFace::eCounterClockwise)
            .setDepthClampEnable(false)
            .setDepthBiasEnable(false)
            .setDepthBiasClamp(0.0f)
            .setDepthBiasSlopeFactor(0.0f)
            .setLineWidth(1.0f)
            .setRasterizerDiscardEnable(false)
            .setPNext(nullptr);
    }

    static vk::PipelineMultisampleStateCreateInfo makeMultisampleState()
    {
        return vk::PipelineMultisampleStateCreateInfo()
            .setRasterizationSamples(vk::SampleCountFlagBits::e1)
            .setSampleShadingEnable(false)
            .setPSampleMask(nullptr)
            .setAlphaToCoverageEnable(false)
            .setAlphaToOneEnable(false)
            .setPNext(nullptr);
    }

    static vk::PipelineDepthStencilStateCreateInfo makeDepthStencilState()
    {
        return vk::PipelineDepthStencilStateCreateInfo()
            .setDepthTestEnable(true)
            .setDepthWriteEnable(true)
            .setDepthCompareOp(vk::CompareOp::eLess)
            .setDepthBoundsTestEnable(false)
            .setStencilTestEnable(false);
    }

    static vk::PipelineViewportStateCreateInfo makeViewportState()
    {
        return vk::PipelineViewportStateCreateInfo()
            .setViewportCount(1)
            .setPViewports(nullptr)
            .setScissorCount(1)
            .setPScissors(nullptr)
            .setPNext(nullptr);
    }

    static vk::PipelineDynamicStateCreateInfo makeDynamicState()
    {
        return vk::PipelineDynamicStateCreateInfo()
            .setDynamicStateCount(0)
            .setPDynamicStates(nullptr)
            .setPNext(nullptr);
    }

    static vk::PipelineColorBlendStateCreateInfo makeColorBlendState()
    {
        return vk::PipelineColorBlendStateCreateInfo()
            .setLogicOp(vk::LogicOp::eClear)
            .setLogicOpEnable(false)
            .setAttachmentCount(0)
            .setPAttachments(nullptr)
            .setBlendConstants({ 0.0f, 0.0f, 0.0f, 0.0f })
            .setPNext(nullptr);
    }

    static vk::PipelineVertexInputStateCreateInfo makeVertexInputState()
    {
        return vk::PipelineVertexInputStateCreateInfo()
            .setVertexAttributeDescriptionCount(0)
            .setPVertexAttributeDescriptions(nullptr)
            .setVertexBindingDescriptionCount(0)
            .setPVertexBindingDescriptions(nullptr)
            .setPNext(nullptr);
    }

    using Clr = vk::ColorComponentFlagBits;
    static vk::PipelineColorBlendAttachmentState makeColorBlendAttachmentState(
        const vk::ColorComponentFlags colorWriteMask      = Clr::eR | Clr::eG | Clr::eB | Clr::eA,
        const vk::Bool32              blendEnable         = false,
        const vk::BlendFactor         srcColorBlendFactor = vk::BlendFactor::eOne,
        const vk::BlendFactor         dstColorBlendFactor = vk::BlendFactor::eZero,
        const vk::BlendOp             colorBlendOp        = vk::BlendOp::eAdd,
        const vk::BlendFactor         srcAlphaBlendFactor = vk::BlendFactor::eOne,
        const vk::BlendFactor         dstAlphaBlendFactor = vk::BlendFactor::eZero,
        const vk::BlendOp             alphaBlendOp        = vk::BlendOp::eAdd)
    {
        return vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(colorWriteMask)
            .setBlendEnable(blendEnable)
            .setSrcColorBlendFactor(srcColorBlendFactor)
            .setDstColorBlendFactor(dstColorBlendFactor)
            .setColorBlendOp(colorBlendOp)
            .setSrcAlphaBlendFactor(srcAlphaBlendFactor)
            .setDstAlphaBlendFactor(dstAlphaBlendFactor)
            .setAlphaBlendOp(alphaBlendOp);
    }
};

struct VulkanShaderCreateInfo
{
    const char*             filePath {};
    vk::ShaderStageFlagBits shaderStage {};
    const char*             entryPoint {"main"};
};

struct VulkanGraphicsPipelineStateInfo
{
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState = VulkanPipelineUtils::makeInputAssemblyState();
    vk::PipelineRasterizationStateCreateInfo rasterizationState = VulkanPipelineUtils::makeRasterizationState();
    vk::PipelineMultisampleStateCreateInfo   multisampleState   = VulkanPipelineUtils::makeMultisampleState();
    vk::PipelineDepthStencilStateCreateInfo  depthStencilState  = VulkanPipelineUtils::makeDepthStencilState();
    vk::PipelineViewportStateCreateInfo      viewportState      = VulkanPipelineUtils::makeViewportState();
    vk::PipelineDynamicStateCreateInfo       dynamicState       = VulkanPipelineUtils::makeDynamicState();
    vk::PipelineColorBlendStateCreateInfo    colorBlendState    = VulkanPipelineUtils::makeColorBlendState();
    vk::PipelineVertexInputStateCreateInfo   vertexInputState   = VulkanPipelineUtils::makeVertexInputState();

    std::vector<vk::DynamicState> dynamicStates = {vk::DynamicState::eScissor, vk::DynamicState::eViewport};

    std::vector<vk::VertexInputAttributeDescription>    attributeDescriptions;
    std::vector<vk::VertexInputBindingDescription>      bindingDescriptions;
    std::vector<vk::PipelineColorBlendAttachmentState>  attachmentStates;

    void update()
    {
        colorBlendState.setAttachmentCount(static_cast<uint32_t>(attachmentStates.size()));
        colorBlendState.setPAttachments(attachmentStates.data());

        vertexInputState.setVertexAttributeDescriptionCount(static_cast<uint32_t>(attributeDescriptions.size()));
        vertexInputState.setPVertexAttributeDescriptions(attributeDescriptions.data());

        vertexInputState.setVertexBindingDescriptionCount(static_cast<uint32_t>(bindingDescriptions.size()));
        vertexInputState.setPVertexBindingDescriptions(bindingDescriptions.data());

        dynamicState.setDynamicStateCount(static_cast<uint32_t>(dynamicStates.size()));
        dynamicState.setPDynamicStates(dynamicStates.data());
    }

    template <class T>
    VulkanGraphicsPipelineStateInfo& addAttributeDescriptions(uint32_t base_location = 0, uint32_t binding = 0)
    {
        for (const auto& attribute : T::attribute_descriptions(base_location, binding))
        {
            attributeDescriptions.push_back(attribute);
        }
        return *this;
    }

    template <class T>
    VulkanGraphicsPipelineStateInfo& addBindingDescriptions(uint32_t binding = 0)
    {
        bindingDescriptions.push_back(T::binding_description(binding));
        return *this;
    }

    VulkanGraphicsPipelineStateInfo& setCullMode(const vk::CullModeFlagBits cullMode)
    {
        rasterizationState.setCullMode(cullMode);
        return *this;
    }

    VulkanGraphicsPipelineStateInfo& setWireframeMode(bool value = true)
    {
        rasterizationState.setPolygonMode(value ? vk::PolygonMode::eFill : vk::PolygonMode::eLine);
        return *this;
    }
};

struct VulkanPipelineCreateInfo
{
    std::vector<vk::PushConstantRange>   pushConstantRanges;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    std::vector<VulkanShaderCreateInfo>  shaderCreateInfos;

    PipelineType                         pipelineType {PipelineType::Graphics};
    vk::RenderPass                       renderPass;
    VulkanGraphicsPipelineStateInfo      graphicsPipelineState;

    VulkanDevice*                        pDevice = nullptr;
    const char*                          debugName;
};

class VulkanPipeline : public RHIPipeline
{
public:
    DISABLE_COPY_CTOR(VulkanPipeline);
    explicit DEF_PRIMARY_CTOR(VulkanPipeline, VulkanPipelineCreateInfo& createInfo);

    ~VulkanPipeline() override = default;

    void bind(RHICommandList* commandList) override
    {
        commandList->as<VulkanCommandList>()->handle().bindPipeline(mBindPoint, mPipeline);
    }

    void bindDescriptorSet(const vk::CommandBuffer& commandBuffer, const vk::DescriptorSet& descriptorSet) {}

    template <typename T>
    void push_constants(const vk::CommandBuffer& commandBuffer, vk::ShaderStageFlags stages, vk::DeviceSize offset, const T* pData) const
    {
        commandBuffer.pushConstants(mPipelineLayout, stages, offset, sizeof(T), pData);
    }

    const vk::Pipeline&       handle() const { return mPipeline; }
    const vk::PipelineLayout& layout() const { return mPipelineLayout; }

    static std::unique_ptr<VulkanPipeline> createTestPipeline(VulkanDevice* pDevice, RHIRenderPass* renderPass)
    {
        VulkanPipelineCreateInfo createInfo = {
            .pushConstantRanges = {},
            .descriptorSetLayouts = {},
            .shaderCreateInfos  = {
                {
                    .filePath = "triangle.vert.spv",
                    .shaderStage = vk::ShaderStageFlagBits::eVertex,
                },
                {
                    .filePath = "triangle.frag.spv",
                    .shaderStage = vk::ShaderStageFlagBits::eFragment,
                }
            },
            .renderPass = renderPass->as<VulkanRenderPass>()->handle(),
            .graphicsPipelineState = VulkanGraphicsPipelineStateInfo({
                .attributeDescriptions = {},
                .bindingDescriptions = {},
                .attachmentStates = { VulkanPipelineUtils::makeColorBlendAttachmentState() },
            }).setCullMode(vk::CullModeFlagBits::eNone),
            .pDevice = pDevice,
            .debugName = "Test Pipeline",
        };

        return std::make_unique<VulkanPipeline>(createInfo);
    }

private:
    static vk::PipelineBindPoint toBindPoint(PipelineType type);

    static std::vector<char> readShaderFile(const char* filePath);

    vk::Pipeline          mPipeline;
    vk::PipelineLayout    mPipelineLayout;
    vk::PipelineBindPoint mBindPoint;

    PipelineType          mPipelineType;

    VulkanDevice*         mDevice;
    const char*           mName;
};