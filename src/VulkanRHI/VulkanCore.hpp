#pragma once

#include "Core/Base.hpp"
#include "RHI/RHIDefinitions.hpp"
#include <vulkan/vulkan.hpp>

class VulkanDevice;
class VulkanCommandQueue;
class VulkanCommandList;
class VulkanSwapchain;

template <typename Chain_t, typename Struct_t>
static void addToPNext(Chain_t& existing, Struct_t& added)
{
    added.setPNext((void*)(existing.pNext));
    existing.setPNext((void*)(&added));
}

#define VK_PRINTLN(MESSAGE) \
    fmt::println("[{}] {}", styled("VulkanRHI", fg(fmt::color::crimson)), MESSAGE)

#pragma region "Vulkan -> RHI Type Conversion"

inline auto toRHI(const vk::Format format)
{
    switch (format)
    {
        case vk::Format::eB8G8R8A8Unorm:        return Format::B8G8R8A8Unorm;
        case vk::Format::eR32G32B32A32Sfloat:   return Format::R32G32B32A32Sfloat;
        case vk::Format::eR32G32B32Sfloat:      return Format::R32G32B32Sfloat;
        case vk::Format::eR32G32Sfloat:         return Format::R32G32Sfloat;
        case vk::Format::eR32Sfloat:            return Format::R32Sfloat;
        default: {
            throw std::runtime_error("Unsupported Format");
        }
    }
}

inline auto toRHI(const vk::Extent2D extent)
{
    return Size2D(extent.width, extent.height);
}

inline auto toRHI(const vk::Offset2D offset)
{
    return Offset2D(offset.x, offset.y);
}

inline auto toRHI(const vk::Rect2D rect)
{
    return Rect2D(toRHI(rect.extent), toRHI(rect.offset));
}

inline auto toRHI(const vk::Viewport& vp)
{
    return Viewport(vp.x, vp.y, vp.width, vp.height, vp.minDepth, vp.maxDepth);
}

#pragma endregion
