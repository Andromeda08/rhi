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

inline vk::Format toVulkan(const Format format)
{
    switch (format)
    {
        case Format::B8G8R8A8Unorm:
            return vk::Format::eB8G8R8A8Unorm;
        case Format::R32G32B32A32Sfloat:
            return vk::Format::eR32G32B32A32Sfloat;
        case Format::R32G32B32Sfloat:
            return vk::Format::eR32G32B32Sfloat;
        case Format::R32G32Sfloat:
            return vk::Format::eR32G32Sfloat;
        case Format::R32Sfloat:
            return vk::Format::eR32Sfloat;
    }

    throw std::runtime_error("Unsupported Format");
}

inline vk::ColorSpaceKHR toVulkan(const ColorSpace colorSpace)
{
    switch (colorSpace)
    {
        case ColorSpace::SrgbNonLinear:
            return vk::ColorSpaceKHR::eSrgbNonlinear;
    }

    throw std::runtime_error("Unsupported ColorSpace");
}

inline vk::Extent2D toVulkan(const Size2D size2d)
{
    return vk::Extent2D { size2d.width, size2d.height };
}
