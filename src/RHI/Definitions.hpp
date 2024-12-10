#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

#include "Macros.hpp"

struct Size2D
{
    uint32_t width  {};
    uint32_t height {};

    Size2D& setWidth(uint32_t value);
    Size2D& setHeight(uint32_t value);

    float aspectRatio() const;
};

struct Size3D
{
    uint32_t width  {};
    uint32_t height {};
    uint32_t depth  {};

    Size3D& setWidth(uint32_t value);
    Size3D& setHeight(uint32_t value);
    Size3D& setDepth(uint32_t value);
};

struct Offset2D
{
    int32_t x {};
    int32_t y {};

    Offset2D& setX(int32_t value);
    Offset2D& setY(int32_t value);
};

struct Rect2D
{
    Offset2D offset {};
    Size2D   size   {};

    Rect2D& setOffset(Offset2D value);
    Rect2D& setSize(Size2D value);
};

struct Viewport
{
    float x         {};
    float y         {};
    float width     {};
    float height    {};
    float minDepth  {};
    float maxDepth  {};

    Viewport& setX(float value);
    Viewport& setY(float value);
    Viewport& setWidth(float value);
    Viewport& setHeight(float value);
    Viewport& setMinDepth(float value);
    Viewport& setMaxDepth(float value);
};

#pragma region "Enums"

enum class RHIInterfaceType
{
    None,
    Vulkan,
    D3D12,
};

enum class RHICommandQueueType
{
    Graphics,
    AsyncCompute,
};

enum class RHIResourceType
{
};

enum class BufferUsageFlags : uint32_t
{
    None            = 0,        // Vulkan Value
    TransferSrc     = 1,        // 1
    TransferDst     = 1 << 1,   // 2
    UniformBuffer   = 1 << 4,   // 16
    StorageBuffer   = 1 << 5,   // 32
    IndexBuffer     = 1 << 6,   // 64
    VertexBuffer    = 1 << 7,   // 128
};
ENUM_FLAGS(BufferUsageFlags);

enum class ColorSpace
{
    SrgbNonLinear,
};

enum class Format
{
    B8G8R8A8Unorm,      // Swapchain Format
    R32G32B32A32Sfloat, // f32 x 4
    R32G32B32Sfloat,    // f32 x 3
    R32G32Sfloat,       // f32 x 2
    R32Sfloat,          // f32 x 1
};

enum class ImageType
{
    Image2D,
    Image3D
};

// TODO: Add required ImageUsageFlags
enum class ImageUsageFlags
{
    None = 0,
};
ENUM_FLAGS(ImageUsageFlags);

enum class ShaderType
{
    None,
    Vertex,
    Fragment,
};

enum class ShaderResourceType
{
    Sampler,
    StorageImage,
    StorageBuffer,
    UniformBuffer,
    CombinedImageSampler,
};

enum class PrimitiveTopology
{
    Triangle,
};

enum class PrimitiveType
{
    TriangleList,
    TriangleStrip
};

#pragma endregion

class RHISwapchain;

class RHIRenderPass;
class RHIFramebuffer;
class RHIPipeline;

#pragma region "RHIFramebuffer"

struct RHIFramebufferAttachment
{
    std::variant<RHISwapchain*> imageView;
    uint32_t                    attachmentIndex  { 0 };
    int32_t                     framebufferIndex { -1 };
};

struct RHIFramebufferCreateInfo
{
    uint32_t                              count       { 0 };
    RHIRenderPass*                        renderPass  { nullptr };
    Size2D                                extent      {0, 0};
    std::vector<RHIFramebufferAttachment> attachments {};
    const char*                           debugName   {};
};

#pragma endregion

#pragma region "RHIRenderPass"

using ClearColorValue = std::array<float, 4>;

struct ClearDepthStencilValue
{
    float    depth   = {};
    uint32_t stencil = {};
};

enum class ImageLayout
{
    ColorAttachmentOptimal,
    PresentSrc,
};

enum class AttachmentLoadOp
{
    Clear,
    Load,
    DontCare,
};

enum class AttachmentStoreOp
{
    None,
    Store,
    DontCare,
};

struct AttachmentDescription
{
    Format               format          { Format::R32G32B32A32Sfloat };
    ImageLayout          finalLayout     { ImageLayout::ColorAttachmentOptimal };
    std::array<float, 4> clearValue      { 0.0f, 0.0f, 0.0f, 1.0f };
    AttachmentLoadOp     loadOp          { AttachmentLoadOp::Clear };
    AttachmentStoreOp    storeOp         { AttachmentStoreOp::DontCare };
    AttachmentLoadOp     stencilLoadOp   { AttachmentLoadOp::DontCare };
    AttachmentStoreOp    stencilStoreOp  { AttachmentStoreOp::DontCare };

    // Ignored when describing a Depth or Resolve attachment
    uint32_t             attachmentIndex { 0 };

    // To be used for D3D12, for now...
    std::variant<std::monostate, RHISwapchain*> attachmentSource = std::monostate {};
};

/**
 * Parameter struct for DynamicRHI::createRenderPass()
 */
struct RHIRenderPassCreateInfo
{
    std::vector<AttachmentDescription>   colorAttachments  {};
    std::optional<AttachmentDescription> depthAttachment   { std::nullopt };
    std::optional<AttachmentDescription> resolveAttachment { std::nullopt };
    Rect2D                               renderArea        {};
    const char*                          debugName;
};

#pragma endregion

#pragma region "RHIPipeline"

enum class PipelineType
{
    Graphics,
    RayTracing,
    Compute,
};

// Shaders
enum class ShaderStage
{
    Vertex,
    Fragment,
};

struct RHIShaderCreateInfo
{
    const char* filePath;
    ShaderStage shaderStage;
};

// Graphics Pipeline State
struct VertexInputAttributeDesc
{
    uint32_t location;
    uint32_t binding;
    Format   format;
    uint32_t offset;
};

enum class VertexInputRate
{
    Vertex,
    Instance,
};

struct VertexInputBindingDesc
{
    uint32_t        binding;
    uint32_t        stride;
    VertexInputRate inputRate;
};

enum class CullMode
{
    None,
    Front,
    Back,
    FrontAndBack,
};

enum class PolygonMode
{
    Fill,
    Line,
};

enum class ColorComponent { R, G, B, A };

enum class BlendFactor
{
    One,
    Zero
};

enum class BlendOp
{
    Add
};

struct BlendRule
{
    BlendFactor srcBlendFactor { BlendFactor::One };
    BlendFactor dstBlendFactor { BlendFactor::Zero };
    BlendOp     blendOp        { BlendOp::Add };
};

struct AttachmentState
{
    std::vector<ColorComponent> colorWriteMask      { ColorComponent::R, ColorComponent::G, ColorComponent::B, ColorComponent::A };
    bool                        blendEnable         { false };
    BlendRule                   blendColor          {};
    BlendRule                   blendAlpha          {};

    static AttachmentState colorsDefault() noexcept
    {
        return {};
    }

    static AttachmentState blendEnabled()  noexcept
    {
        return {
            .blendEnable = true,
        };
    }
};

/**
 * Parameter struct for configuring a Graphics Pipeline
 */
struct RHIGraphicsPipelineState
{
    CullMode                              cullMode              { CullMode::Back };
    PolygonMode                           polygonMode           { PolygonMode::Fill };
    std::vector<VertexInputAttributeDesc> vertexInputAttributes {};
    std::vector<VertexInputBindingDesc>   vertexInputBindings   {};
    std::vector<AttachmentState>          attachmentStates      {};
};

/**
 * Parameter struct for DynamicRHI::createPipeline()
 */
struct RHIPipelineCreateInfo
{
    std::vector<RHIShaderCreateInfo> shaderCreateInfos     {};
    RHIGraphicsPipelineState         graphicsPipelineState {};
    RHIRenderPass*                   renderPass            { nullptr };
    PipelineType                     pipelineType          { PipelineType::Graphics };
    const char*                      debugName;
};

#pragma endregion

/**
 * Buffers
 */
#pragma region "Buffers"

enum RHIBufferType
{
    Index,
    Staging,
    Storage,
    Uniform,
    Vertex,
};

struct RHIBufferCreateInfo
{
    uint64_t        bufferSize;
    RHIBufferType   bufferType;
    void*           pData {nullptr};
    const char*     debugName;
};

#pragma endregion
