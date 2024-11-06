#pragma once

#include <cstdint>
#include "Macros.hpp"

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

/**
 * Buffers
 */
#pragma region "Buffers"

enum RHIBufferType
{
    eIndex,
    eStaging,
    eStorage,
    eUniform,
    eVertex,
};

struct RHIBufferCreateInfo
{
    uint64_t        bufferSize;
    RHIBufferType   bufferType;
    const char*     debugName;
};

class RHIBuffer;

#pragma endregion

struct Size2D
{
    uint32_t width;
    uint32_t height;

    Size2D& setWidth(uint32_t value);
    Size2D& setHeight(uint32_t value);

    float aspectRatio() const;
};

struct Size3D
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;

    auto setWidth(uint32_t value);
    auto setHeight(uint32_t value);
    auto setDepth(uint32_t value);
};

struct Offset2D
{
    int32_t x;
    int32_t y;

    Offset2D& setX(int32_t value);
    Offset2D& setY(int32_t value);
};

struct Rect2D
{
    Size2D size;
    Offset2D offset;

    Rect2D& setSize(Size2D value);
    Rect2D& setOffset(Offset2D value);
};

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;

    auto setX(float value);
    auto setY(float value);
    auto setWidth(float value);
    auto setHeight(float value);
    auto setMinDepth(float value);
    auto setMaxDepth(float value);
};
