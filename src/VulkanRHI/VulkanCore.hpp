#pragma once

#include "Core/Base.hpp"
#include "RHI/DynamicRHI.hpp"
#include <vulkan/vulkan.hpp>

template <typename Chain_t, typename Struct_t>
static void addToPNext(Chain_t& existing, Struct_t& added)
{
    added.setPNext((void*)(existing.pNext));
    existing.setPNext((void*)(&added));
}
