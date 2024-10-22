#pragma once

#include "Core/Base.hpp"


struct Size2D
{
    uint32_t width;
    uint32_t height;

    Size2D(const uint32_t _width = {}, const uint32_t _height = {})
    : width(_width), height(_height)
    {
    }

    auto setWidth(const uint32_t value)
    {
        width = value;
        return *this;
    }

    auto setHeight(const uint32_t value)
    {
        height = value;
        return *this;
    }
};

struct Size3D
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;

    Size3D(const uint32_t _width = {}, const uint32_t _height = {}, const uint32_t _depth = {})
    : width(_width), height(_height), depth(_depth)
    {
    }

    auto setWidth(const uint32_t value)
    {
        width = value;
        return *this;
    }

    auto setHeight(const uint32_t value)
    {
        height = value;
        return *this;
    }

    auto setDepth(const uint32_t value)
    {
        depth = value;
        return *this;
    }
};

struct Offset2D
{
    int32_t x;
    int32_t y;

    Offset2D(const int32_t _x = {}, const int32_t _y = {})
    : x(_x), y(_y)
    {
    }

    auto setX(const int32_t value)
    {
        x = value;
        return *this;
    }

    auto setY(const int32_t value)
    {
        y = value;
        return *this;
    }
};

struct Rect2D
{
    Size2D size;
    Offset2D offset;

    explicit Rect2D(const Size2D _size = {}, const Offset2D _offset = {})
    : size(_size), offset(_offset)
    {
    }
};

struct Viewport
{
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;

    Viewport(
        const float _x = {}, const float _y = {}, const float _width = {},
        const float _height = {}, const float _minDepth = {}, const float _maxDepth = {}
    ) : x(_x), y(_y), width(_width), height(_height), minDepth(_minDepth), maxDepth(_maxDepth)
    {
    }

    auto setX(const float value)
    {
        x = value;
        return *this;
    }

    auto setY(const float value)
    {
        y = value;
        return *this;
    }

    auto setWidth(const float value)
    {
        width = value;
        return *this;
    }

    auto setHeight(const float value)
    {
        height = value;
        return *this;
    }

    auto setMinDepth(const float value)
    {
        minDepth = value;
        return *this;
    }

    auto setMaxDepth(const float value)
    {
        maxDepth = value;
        return *this;
    }
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
