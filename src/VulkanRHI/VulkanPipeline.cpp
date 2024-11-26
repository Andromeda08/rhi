#include "VulkanPipeline.hpp"

#include <fstream>

VulkanPipeline::VulkanPipeline(VulkanPipelineCreateInfo& createInfo)
: mBindPoint(toBindPoint(createInfo.pipelineType)), mPipelineType(createInfo.pipelineType)
, mDevice(createInfo.pDevice), mName(createInfo.debugName)
{
    if (createInfo.shaderCreateInfos.empty())
    {
        throw std::runtime_error(fmt::format("Can't create Pipeline with no shaders specified"));
    }

    auto& graphicsPipelineState = createInfo.graphicsPipelineState;
    graphicsPipelineState.update();

    const auto layoutCreateInfo = vk::PipelineLayoutCreateInfo()
        .setSetLayoutCount(createInfo.descriptorSetLayouts.size())
        .setPSetLayouts(createInfo.descriptorSetLayouts.data())
        .setPushConstantRangeCount(createInfo.pushConstantRanges.size())
        .setPPushConstantRanges(createInfo.pushConstantRanges.data());

    std::vector<vk::ShaderModule> shaders(createInfo.shaderCreateInfos.size());
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStageInfos(createInfo.shaderCreateInfos.size());

    try
    {
        mPipelineLayout = mDevice->handle().createPipelineLayout(layoutCreateInfo);

        for (const auto& [index, shaderInfo] : std::views::enumerate(createInfo.shaderCreateInfos))
        {
            const auto shaderSrc = readShaderFile(shaderInfo.filePath);
            auto shaderModuleCreateInfo = vk::ShaderModuleCreateInfo()
                .setCodeSize(sizeof(char) * shaderSrc.size())
                .setPCode(reinterpret_cast<const uint32_t*>(shaderSrc.data()));

            shaders[index] = mDevice->handle().createShaderModule(shaderModuleCreateInfo);

            shaderStageInfos[index] = vk::PipelineShaderStageCreateInfo()
                .setStage(shaderInfo.shaderStage)
                .setModule(shaders[index])
                .setPName(shaderInfo.entryPoint);
        }
    } catch (const vk::SystemError& error) {
        fmt::println("Failed to create PipelineLayout: {}", error.what());
        throw;
    }

    const auto graphicsPipelineCreateInfo = vk::GraphicsPipelineCreateInfo()
            .setPInputAssemblyState(&graphicsPipelineState.inputAssemblyState)
            .setPRasterizationState(&graphicsPipelineState.rasterizationState)
            .setPMultisampleState(&graphicsPipelineState.multisampleState)
            .setPDepthStencilState(&graphicsPipelineState.depthStencilState)
            .setPViewportState(&graphicsPipelineState.viewportState)
            .setPDynamicState(&graphicsPipelineState.dynamicState)
            .setPColorBlendState(&graphicsPipelineState.colorBlendState)
            .setPVertexInputState(&graphicsPipelineState.vertexInputState)
            .setStageCount(shaderStageInfos.size())
            .setPStages(shaderStageInfos.data())
            .setLayout(mPipelineLayout)
            .setRenderPass(createInfo.renderPass)
            .setPNext(nullptr);

    try
    {
        mPipeline = mDevice->handle().createGraphicsPipeline(nullptr, graphicsPipelineCreateInfo).value;
    } catch (const vk::SystemError& error) {
        fmt::println("Failed to create Pipeline: {}", error.what());
        throw;
    }
}

std::unique_ptr<VulkanPipeline> VulkanPipeline::createVulkanPipeline(VulkanPipelineCreateInfo& createInfo)
{
    return std::make_unique<VulkanPipeline>(createInfo);
}

vk::PipelineBindPoint VulkanPipeline::toBindPoint(const PipelineType type)
{
    using enum PipelineType;
    switch (type)
    {
        case Compute:    return vk::PipelineBindPoint::eCompute;
        case Graphics:   return vk::PipelineBindPoint::eGraphics;
        case RayTracing: return vk::PipelineBindPoint::eRayTracingKHR;
    }
    throw std::runtime_error("");
}

std::vector<char> VulkanPipeline::readShaderFile(const char* filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error(fmt::format("Failed to open file: {}!", filePath));
    }

    const size_t fileSize = file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

