#include "Definitions.hpp"

auto Size2D::setWidth(const uint32_t value)
{
    width = value;
    return *this;
}

auto Size2D::setHeight(const uint32_t value)
{
    height = value;
    return *this;
}

float Size2D::aspectRatio() const
{
    return static_cast<float>(width) / static_cast<float>(height);
}

auto Size3D::setWidth(const uint32_t value)
{
    width = value;
    return *this;
}

auto Size3D::setHeight(const uint32_t value)
{
    height = value;
    return *this;
}

auto Size3D::setDepth(const uint32_t value)
{
    depth = value;
    return *this;
}

auto Offset2D::setY(const int32_t value)
{
    y = value;
    return *this;
}

auto Offset2D::setX(const int32_t value)
{
    x = value;
    return *this;
}

auto Rect2D::setSize(const Size2D value)
{
    size = value;
    return *this;
}

auto Rect2D::setOffset(const Offset2D value)
{
    offset = value;
    return *this;
}

auto Viewport::setX(const float value)
{
    x = value;
    return *this;
}

auto Viewport::setY(const float value)
{
    y = value;
    return *this;
}

auto Viewport::setWidth(const float value)
{
    width = value;
    return *this;
}

auto Viewport::setHeight(const float value)
{
    height = value;
    return *this;
}

auto Viewport::setMinDepth(const float value)
{
    minDepth = value;
    return *this;
}

auto Viewport::setMaxDepth(const float value)
{
    maxDepth = value;
    return *this;
}
