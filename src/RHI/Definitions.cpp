#include "Definitions.hpp"

Size2D& Size2D::setWidth(const uint32_t value)
{
    width = value;
    return *this;
}

Size2D& Size2D::setHeight(const uint32_t value)
{
    height = value;
    return *this;
}

float Size2D::aspectRatio() const
{
    return static_cast<float>(width) / static_cast<float>(height);
}

Size3D& Size3D::setWidth(const uint32_t value)
{
    width = value;
    return *this;
}

Size3D& Size3D::setHeight(const uint32_t value)
{
    height = value;
    return *this;
}

Size3D& Size3D::setDepth(const uint32_t value)
{
    depth = value;
    return *this;
}

Offset2D& Offset2D::setY(const int32_t value)
{
    y = value;
    return *this;
}

Offset2D& Offset2D::setX(const int32_t value)
{
    x = value;
    return *this;
}

Rect2D& Rect2D::setSize(const Size2D value)
{
    size = value;
    return *this;
}

Rect2D& Rect2D::setOffset(const Offset2D value)
{
    offset = value;
    return *this;
}

Viewport& Viewport::setX(const float value)
{
    x = value;
    return *this;
}

Viewport& Viewport::setY(const float value)
{
    y = value;
    return *this;
}

Viewport& Viewport::setWidth(const float value)
{
    width = value;
    return *this;
}

Viewport& Viewport::setHeight(const float value)
{
    height = value;
    return *this;
}

Viewport& Viewport::setMinDepth(const float value)
{
    minDepth = value;
    return *this;
}

Viewport& Viewport::setMaxDepth(const float value)
{
    maxDepth = value;
    return *this;
}
