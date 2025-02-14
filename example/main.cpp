#include <RHI.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include "util.hpp"
#include "Scene/Geometry.hpp"
#include "WSI/Window.hpp"

std::unique_ptr<Window> gWindow;

RHIInterfaceType parseApiArgument(const std::string& arg) noexcept
{
    if (arg == "--d3d12")  return RHIInterfaceType::D3D12;
    if (arg == "--vulkan") return RHIInterfaceType::Vulkan;

    fmt::println("[{}] Invalid API argument: {}, defaulting to Vulkan",
        styled("Warning", fg(fmt::color::light_yellow)), arg);

    return RHIInterfaceType::Vulkan;
}

int main(const int argc, char** argv)
{
    auto api = RHIInterfaceType::Vulkan;
    if (argc >= 2)
    {
        const std::string apiArg = argv[1];
        api = parseApiArgument(apiArg);
    }

    gWindow = Window::createWindow({
        .resolution = { 1600, 900 },
        .title = fmt::format("RHI Window ({})", toString(api)),
    });

    gRHI = rhiFactory(api, gWindow.get());

    const std::unique_ptr<Geometry> cubeGeometry = std::make_unique<Cube>();

    #pragma region "Cube Vertex and Index Buffer"
    const auto vertexBuffer = gRHI->createBuffer({
        .bufferSize = cubeGeometry->vertexCount() * sizeof(BasicVertex),
        .bufferType = Vertex,
        .debugName  = "Cube Vertices",
    });

    const auto vertexStaging = gRHI->createBuffer({
        .bufferSize = vertexBuffer->getSize(),
        .bufferType = Staging,
        .debugName  = "Cube Vertices Staging",
    });
    vertexStaging->setData(cubeGeometry->getVertices().data(), cubeGeometry->vertexCount() * sizeof(BasicVertex));

    const auto indexBuffer = gRHI->createBuffer({
        .bufferSize = cubeGeometry->indexCount() * sizeof(uint32_t),
        .bufferType = Index,
        .debugName  = "Cube Indices",
    });

    const auto indexStaging = gRHI->createBuffer({
        .bufferSize = indexBuffer->getSize(),
        .bufferType = Staging,
        .debugName  = "Cube Indices Staging",
    });
    indexStaging->setData(cubeGeometry->getIndices().data(), cubeGeometry->indexCount() * sizeof(uint32_t));

    gRHI->getGraphicsQueue()->executeSingleTimeCommand([&](RHICommandList* commandList) {
        vertexBuffer->uploadData({
            .pData = cubeGeometry->getVertices().data(),
            .dataSize = cubeGeometry->vertexCount() * sizeof(BasicVertex),
            .pCommandList = commandList,
            .pStagingBuffer = vertexStaging.get(),
        });
        indexBuffer->uploadData({
            .pData = cubeGeometry->getIndices().data(),
            .dataSize = cubeGeometry->indexCount() * sizeof(uint32_t),
            .pCommandList = commandList,
            .pStagingBuffer = indexStaging.get(),
        });
    });
    #pragma endregion

    #pragma region "Render Targets Setup"
    const auto depthTexture = gRHI->createTexture({
        .size = gRHI->getSwapchain()->getSize(),
        .format = Format::D32Sfloat,
        .sampled = false,
        .debugName = "Depth Texture"
    });

    const auto testRenderPass = gRHI->createRenderPass({
        .colorAttachments = {
            {
                .format = gRHI->getSwapchain()->getFormat(),
                .finalLayout = ImageLayout::PresentSrc,
                .attachmentSource = gRHI->getSwapchain(),
            },
        },
        .depthAttachment = AttachmentDescription({
            .format = Format::D32Sfloat,
            .finalLayout = ImageLayout::DepthAttachmentOptimal,
            .depthClearValue = 1.0f,
            .stencilClearValue = 0,
            .attachmentSource = depthTexture.get(),
        }),
        .renderArea = {{0, 0}, gRHI->getSwapchain()->getSize()},
        .debugName  = "Test RenderPass",
    });

    const auto testFramebuffers = gRHI->createFramebuffer({
        .count = 2,
        .renderPass = testRenderPass.get(),
        .extent = gRHI->getSwapchain()->getSize(),
        .attachments = {
            { gRHI->getSwapchain(), 0, 0 },
            { gRHI->getSwapchain(), 0, 1 },
            { depthTexture.get(), 1 },
        },
        .debugName = "Test Framebuffer",
    });
    #pragma endregion

    #pragma region "(Basic Forward) Pipeline"
    const auto fwdPipeline = gRHI->createPipeline({
        .shaderCreateInfos = {
            { (api == RHIInterfaceType::Vulkan) ? "forward.vert.spv" : "forward.vert.dxil", ShaderStage::Vertex   },
            { (api == RHIInterfaceType::Vulkan) ? "forward.frag.spv" : "forward.frag.dxil", ShaderStage::Fragment }
        },
        .graphicsPipelineState = {
            .cullMode = CullMode::Back,
            .vertexInputAttributes = {
                { 0, 0, Format::R32G32B32Sfloat, offsetof(BasicVertex, position), "POSITION", 0 },
                { 1, 0, Format::R32G32B32Sfloat, offsetof(BasicVertex, normal), "NORMAL", 0 },
                { 2, 0, Format::R32G32Sfloat, offsetof(BasicVertex, uv), "TEXCOORD", 0 },
            },
            .vertexInputBindings = {
                { 0, sizeof(BasicVertex), VertexInputRate::Vertex, 0 },
            },
            .attachmentStates = { AttachmentState::colorsDefault() },
        },
        .renderPass = testRenderPass.get(),
        .pipelineType = PipelineType::Graphics,
        .debugName = "Forward Pipeline",
    });
    #pragma endregion

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

            fwdPipeline->bind(cmd);
            cmd->bindVertexBuffer(vertexBuffer.get());
            cmd->bindIndexBuffer(indexBuffer.get());
            cmd->drawIndexed(cubeGeometry->indexCount(), 1, 0, 0, 0);
        });
        commandList->end();

        frameInfo.addCommandLists({ commandList });
        gRHI->submitFrame(frameInfo);
    }

    return 0;
}
