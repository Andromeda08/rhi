#include "WSI/Window.hpp"
#include "RHI/DynamicRHI.hpp"
#include "VulkanRHI/VulkanRHI.hpp"
#ifdef D3D12_RHI_ENABLED
    #include "D3D12RHI/D3D12RHI.hpp"
#endif

std::unique_ptr<Window> gWindow;

int main(const int argc, char** argv)
{
    auto api = RHIInterfaceType::Vulkan;
    if (argc >= 2)
    {
        const std::string apiArg = argv[1];

        if   (apiArg == "D3D12") api = RHIInterfaceType::D3D12;
        else                     api = RHIInterfaceType::Vulkan;
    }

    const auto windowCreateInfo = WindowCreateInfo {
        .resolution = { 1280, 720 },
        .title = "RHI Example Window",
    };
    gWindow = std::make_unique<Window>(windowCreateInfo);

    switch (api)
    {
        case RHIInterfaceType::Vulkan: {
            gRHI = VulkanRHI::createVulkanRHI({
                .pWindow = gWindow.get(),
            });
            break;
        }
        case RHIInterfaceType::D3D12: {
#ifndef __APPLE__
            gRHI = D3D12RHI::createD3D12RHI({
                .pWindow = gWindow.get(),
            });
#else
            fmt::println("[{}] {}",
                styled("Elysia", fg(fmt::color::medium_purple)),
                fmt::format("{} Backend not supported on {}",
                    styled("D3D12", fg(fmt::color::green_yellow)),
                    styled("macOS", fg(fmt::color::medium_purple))));
            return 1;
#endif
            break;
        }
        default:
            return 0;
    }

    std::vector testData = { 0.0f, 0.25f, 0.5f, 0.75f, 1.0f };

    auto testBuffer = gRHI->createBuffer({
        .bufferSize = 128,
        .bufferType = Uniform,
        .pData      = testData.data(),
        .debugName  = "Test Buffer",
    });

    const auto testRenderPass = gRHI->createRenderPass({
        .colorAttachments = {
            {
                .format = gRHI->getSwapchain()->getFormat(),
                .finalLayout = ImageLayout::PresentSrc,
            }},
        .renderArea = {{0, 0}, gRHI->getSwapchain()->getSize()},
        .debugName  = "Test RenderPass",
    });

    const auto testFramebuffers = gRHI->createFramebuffer({
        .count = 2,
        .renderPass = testRenderPass.get(),
        .extent = gRHI->getSwapchain()->getSize(),
        .attachments =
        {
            { gRHI->getSwapchain(), 0, 0 },
            { gRHI->getSwapchain(), 0, 1 }
        },
        .debugName = "Test Framebuffer",
    });

    const auto testPipeline = gRHI->createPipeline({
        .shaderCreateInfos = {
            { "triangle.vert.spv", ShaderStage::Vertex   },
            { "triangle.frag.spv", ShaderStage::Fragment }
        },
        .graphicsPipelineState = {
            .cullMode = CullMode::None,
            .attachmentStates = { AttachmentState::colorsDefault() },
        },
        .renderPass = testRenderPass.get(),
        .pipelineType = PipelineType::Graphics,
        .debugName = "Test Pipeline",
    });

    while (!gWindow->shouldClose())
    {
        glfwPollEvents();

        auto frameInfo = gRHI->beginFrame({
            .useSwapchain = true
        });

        auto* commandList = gRHI->getGraphicsQueue()->getCommandList(frameInfo.getCurrentFrame());
        commandList->begin();
        testRenderPass->execute(commandList, testFramebuffers->getFramebuffer(frameInfo.getCurrentFrame()), [&](RHICommandList* cmd)
        {
            gRHI->getSwapchain()->setScissorViewport(cmd);

            testPipeline->bind(commandList);
            cmd->draw(3, 1, 0, 0);
        });
        commandList->end();

        frameInfo.addCommandLists({ commandList });
        gRHI->submitFrame(frameInfo);
    }

    return 0;
}
