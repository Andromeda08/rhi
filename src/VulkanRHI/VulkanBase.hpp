#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <ranges>
#include <set>
#include <variant>
#include <vector>
#include <vulkan/vulkan.hpp>

#include <fmt/format.h>
#include <fmt/color.h>

#include "RHI/Macros.hpp"

#define VK_PREFIX "Vulkan"
#define VK_STYLED_PREFIX styled(VK_PREFIX, fg(fmt::color::crimson))
#define STYLED_DEBUG styled("D", fg(fmt::color::crimson))
#define STYLED_VERBOSE styled("V", fg(fmt::color::medium_purple))

#ifdef VULKAN_VERBOSE_LOGGING
    #define VK_VERBOSE(MESSAGE) fmt::println("[{}] {}", VK_STYLED_PREFIX, MESSAGE)
#else
    #define VK_VERBOSE(MESSAGE)
#endif

#ifdef VULKAN_DEBUGGING_ENABLED
    #define VK_DEBUG(MESSAGE) fmt::println("[{}] {}", VK_STYLED_PREFIX, MESSAGE)
#else
    #define VK_DEBUG(MESSAGE)
#endif

#define VK_PRINTLN(MESSAGE) fmt::println("[{}] {}", VK_STYLED_PREFIX, MESSAGE)

/**
 * Wraps the given statement in a try/catch block for Vulkan specific errors.
 * For debug mode only checks use `VK_CHECK_DEBUG`
 */
#define VK_CHECK(C) try { C } catch (const vk::SystemError& error) { VK_DEBUG(error.what()); throw; }

#ifdef VULKAN_DEBUGGING_ENABLED
    #define VK_CHECK_DEBUG(C) VK_CHECK(C)
#else
    #define VK_CHECK_DEBUG(C) C
#endif

#pragma region "Vulkan utility functions"

template <typename Chain_t, typename Struct_t>
static void addToPNext(Chain_t& existing, Struct_t& added)
{
    added.setPNext((void*)(existing.pNext));
    existing.setPNext((void*)(&added));
}

inline int32_t findExtension(const char* extensionName, const std::vector<vk::ExtensionProperties>& extensionProperties)
{
    if (extensionName == nullptr)
    {
        return -1;
    }

    for (int32_t i = 0; i < extensionProperties.size(); i++)
    {
        if (!std::strcmp(extensionProperties[i].extensionName, extensionName))
        {
            return i;
        }
    }
    return -1;
}

inline std::vector<const char*> getSupportedLayers(const std::vector<const char*>& requestedLayers, const std::vector<vk::LayerProperties>& driverLayers)
{
    std::set uniqueRequestedLayers(std::begin(requestedLayers), std::end(requestedLayers));

    auto supportedLayers = uniqueRequestedLayers | std::views::filter([&](const char* value) {
        const bool found = std::ranges::find_if(driverLayers, [&](const vk::LayerProperties& layerProperties) {
            return std::strcmp(value, layerProperties.layerName);
        }) != std::end(driverLayers);
        return found;
    });

    return {std::begin(supportedLayers), std::end(supportedLayers)};
}

inline std::vector<const char*> getSupportedExtensions(const std::vector<const char*>& requestedExtensions, const std::vector<vk::ExtensionProperties>& driverExtensions)
{
    std::set uniqueRequestedExtensions(std::begin(requestedExtensions), std::end(requestedExtensions));

    auto supportedExtensions = uniqueRequestedExtensions | std::views::filter([&](const char* value) {
        const bool found = std::ranges::find_if(driverExtensions, [&](const vk::ExtensionProperties& extensionProperties) {
            return std::strcmp(value, extensionProperties.extensionName);
        }) != std::end(driverExtensions);
        return found;
    });

    return {std::begin(supportedExtensions), std::end(supportedExtensions)};
}

#pragma endregion

#include "RHI/Definitions.hpp"

#pragma region "RHI to Vulkan Type Conversion"

inline vk::Extent2D toVulkan(const Size2D size2d)
{
    return { size2d.width, size2d.height };
}

inline vk::Offset2D toVulkan(const Offset2D offset2d)
{
    return { offset2d.x, offset2d.y };
}

inline vk::Rect2D toVulkan(const Rect2D rect2d)
{
    return { toVulkan(rect2d.offset), toVulkan(rect2d.size) };
}

inline vk::Format toVulkan(const Format format)
{
    switch (format)
    {
        case Format::B8G8R8A8Unorm:        return vk::Format::eB8G8R8A8Unorm;
        case Format::R32G32B32A32Sfloat:   return vk::Format::eR32G32B32A32Sfloat;
        case Format::R32G32B32Sfloat:      return vk::Format::eR32G32B32Sfloat;
        case Format::R32G32Sfloat:         return vk::Format::eR32G32Sfloat;
        case Format::R32Sfloat:            return vk::Format::eR32Sfloat;
    }
    throw std::exception();
}

// As not all values are supported, the method returns 'vk::ImageLayout::eUndefined' as default
inline vk::ImageLayout toVulkan(const ImageLayout imageLayout) noexcept
{
    using enum ImageLayout;
    switch (imageLayout)
    {
        case ColorAttachmentOptimal: return vk::ImageLayout::eColorAttachmentOptimal;
        case PresentSrc:             return vk::ImageLayout::ePresentSrcKHR;
    }
    return vk::ImageLayout::eUndefined;
}

inline vk::AttachmentLoadOp toVulkan(const AttachmentLoadOp loadOp)
{
    using enum AttachmentLoadOp;
    switch (loadOp)
    {
        case Clear:     return vk::AttachmentLoadOp::eClear;
        case Load:      return vk::AttachmentLoadOp::eLoad;
        case DontCare:  return vk::AttachmentLoadOp::eDontCare;
    }
    throw std::exception();
}

inline vk::AttachmentStoreOp toVulkan(const AttachmentStoreOp storeOp)
{
    using enum AttachmentStoreOp;
    switch (storeOp)
    {
        case None:      return vk::AttachmentStoreOp::eNone;
        case Store:     return vk::AttachmentStoreOp::eStore;
        case DontCare:  return vk::AttachmentStoreOp::eDontCare;
    }
    throw std::exception();
}

#pragma endregion

#pragma region "Vulkan to RHI Type Conversion"

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
    return Size2D {
        .width = extent.width,
        .height = extent.height
    };
}

inline auto toRHI(const vk::Offset2D offset)
{
    return Offset2D {
        .x = offset.x,
        .y = offset.y
    };
}

inline auto toRHI(const vk::Rect2D rect)
{
    return Rect2D {
        .size = toRHI(rect.extent),
        .offset = toRHI(rect.offset)
    };
}

inline auto toRHI(const vk::Viewport& vp)
{
    return Viewport {
        .x = vp.x,
        .y = vp.y,
        .width = vp.width,
        .height = vp.height,
        .minDepth = vp.minDepth,
        .maxDepth = vp.maxDepth
    };
}

#pragma endregion
