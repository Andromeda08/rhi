#pragma once

#include "Core/EnumFlags.hpp"

struct Size2D
{
    uint32_t width;
    uint32_t height;
};

struct Size3D
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
};

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
