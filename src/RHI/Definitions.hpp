#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "Macros.hpp"

rhi_BEGIN_NAMESPACE;

/**
 * RHI Forward Declarations
 */

class DynamicRHI;
class RHIBuffer;
class RHICommandList;
class RHICommandQueue;
class RHIFramebuffer;
class RHIFramebufferHandle;
class RHIPipeline;
class RHIRenderPass;
class RHISwapchain;
class RHITexture;

struct Frame;

/**
 * RHI: Common structs and types
 */
#pragma region "common"

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

struct RHIFrameBeginInfo
{
    bool useSwapchain = true;
};

#pragma endregion

/**
 * RHI: Enums
 */
#pragma region "enums"

enum class RHIInterfaceType
{
    None,
    Vulkan,
    D3D12,
};

enum RHIBufferType
{
    Index,
    Staging,
    Storage,
    Uniform,
    Vertex,
};

enum class RHICommandQueueType
{
    Graphics,
    AsyncCompute,
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

enum class ColorComponent
{
    R,
    G,
    B,
    A,
};

enum class BlendFactor
{
    One,
    Zero
};

enum class BlendOp
{
    Add
};

enum class CullMode
{
    None,
    Front,
    Back,
    FrontAndBack,
};

enum class ColorSpace
{
    SrgbNonLinear,
};

enum class Format
{
    B8G8R8A8Unorm,
    R32G32B32A32Sfloat,
    R32G32B32Sfloat,
    R32G32Sfloat,
    R32Sfloat,
    D32Sfloat,
};

enum class ImageType
{
    Image2D
};

enum class ImageLayout
{
    ColorAttachmentOptimal,
    DepthAttachmentOptimal,
    PresentSrc,
};

enum class PipelineType
{
    Graphics,
    RayTracing,
    Compute,
};

enum class PolygonMode
{
    Fill,
    Line,
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

enum class ShaderStage
{
    Vertex,
    Fragment,
};

enum class VertexInputRate
{
    Vertex,
    Instance,
};

#pragma endregion

/**
 * RHI: Enum to string conversion
 */
#pragma region "enum to string"

inline std::string toString(const RHIInterfaceType interfaceType) noexcept
{
    switch (interfaceType)
    {
        case RHIInterfaceType::Vulkan:  return "Vulkan";
        case RHIInterfaceType::D3D12:   return "D3D12";
        default:                        return "No API";
    }
}

#pragma endregion


/**
 * RHI: Framebuffer
 */
#pragma region "framebuffer"

struct RHIFramebufferAttachment
{
    std::variant<RHISwapchain*, RHITexture*> imageView;
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

/**
 * RHI: RenderPass
 */
#pragma region "renderpass"

using ClearColorValue = std::array<float, 4>;

struct ClearDepthStencilValue
{
    float    depth   = {};
    uint32_t stencil = {};
};

using AttachmentSource = std::variant<std::monostate, RHITexture*, RHISwapchain*>;

struct AttachmentDescription
{
    Format               format             = Format::R32G32B32A32Sfloat ;
    ImageLayout          finalLayout        = ImageLayout::ColorAttachmentOptimal;
    std::array<float, 4> clearValue         = { 0.0f, 0.0f, 0.0f, 1.0f };
    AttachmentLoadOp     loadOp             = AttachmentLoadOp::Clear;
    AttachmentStoreOp    storeOp            = AttachmentStoreOp::DontCare;
    AttachmentLoadOp     stencilLoadOp      = AttachmentLoadOp::DontCare;
    AttachmentStoreOp    stencilStoreOp     = AttachmentStoreOp::DontCare;

    float                depthClearValue    = 1.0f;
    uint32_t             stencilClearValue  = 0;

    // Ignored when describing a Depth or Resolve attachment
    uint32_t             attachmentIndex    = 0;

    // Used only by the D3D12 Backend
    AttachmentSource     attachmentSource   = std::monostate {};
};

struct RHIRenderPassCreateInfo
{
    std::vector<AttachmentDescription>   colorAttachments  = {};
    std::optional<AttachmentDescription> depthAttachment   = { std::nullopt };
    std::optional<AttachmentDescription> resolveAttachment = { std::nullopt };
    Rect2D                               renderArea        = {};
    const char*                          debugName         = {};
};

#pragma endregion

/**
 * RHI: Pipeline
 */
#pragma region "pipeline"

struct RHIShaderCreateInfo
{
    const char* filePath;
    ShaderStage shaderStage;
};

struct VertexInputAttributeDesc
{
    uint32_t    location;
    uint32_t    binding;
    Format      format;
    uint32_t    offset;

    std::string semanticName  {};       // D3D12 only
    uint32_t    semanticIndex {};       // D3D12 only
    bool        appendAligned {true};   // D3D12 only
};

struct VertexInputBindingDesc
{
    uint32_t        binding;            // Vulkan only
    uint32_t        stride;             // Vulkan only
    VertexInputRate inputRate;          // Shared
    uint32_t        instanceStepRate;   // D3D12 only
};

struct BlendRule
{
    BlendFactor srcBlendFactor = BlendFactor::One;
    BlendFactor dstBlendFactor = BlendFactor::Zero;
    BlendOp     blendOp        = BlendOp::Add;
};

struct AttachmentState
{
    using enum ColorComponent;

    std::vector<ColorComponent> colorWriteMask = { R, G, B, A };
    bool                        blendEnable    = false;
    BlendRule                   blendColor     = {};
    BlendRule                   blendAlpha     = {};

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

struct RHIGraphicsPipelineState
{
    CullMode                              cullMode              = CullMode::Back;
    PolygonMode                           polygonMode           = PolygonMode::Fill;
    bool                                  depthTest             = true;
    std::vector<VertexInputAttributeDesc> vertexInputAttributes = {};
    std::vector<VertexInputBindingDesc>   vertexInputBindings   = {};
    std::vector<AttachmentState>          attachmentStates      = {};
};

struct RHIPipelineCreateInfo
{
    std::vector<RHIShaderCreateInfo> shaderCreateInfos     = {};
    RHIGraphicsPipelineState         graphicsPipelineState = {};
    RHIRenderPass*                   renderPass            = nullptr;
    PipelineType                     pipelineType          = PipelineType::Graphics;
    const char*                      debugName             = {};
};

#pragma endregion

/**
 * RHI: Buffers
 */
#pragma region "buffers"

struct RHIBufferCreateInfo
{
    uint64_t        bufferSize = 0;
    RHIBufferType   bufferType;
    void*           pData      = nullptr;
    std::string     debugName  = {};
};

#pragma endregion

/**
 * Utility Methods
 */
#pragma region "Utility Methods"

inline bool isDepthFormat(const Format format) noexcept
{
    return format == Format::D32Sfloat;
}

#pragma endregion

rhi_END_NAMESPACE;
