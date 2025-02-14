#pragma once

#include "RHI/Definitions.hpp"
#include "RHI/DynamicRHI.hpp"
#include "RHI/Frame.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHICommandList.hpp"
#include "RHI/RHICommandQueue.hpp"
#include "RHI/RHIFramebuffer.hpp"
#include "RHI/RHIPipeline.hpp"
#include "RHI/RHIRenderPass.hpp"
#include "RHI/RHISwapchain.hpp"
#include "RHI/RHITexture.hpp"
#include "RHI/RHIWindow.hpp"

rhi_BEGIN_NAMESPACE;

/**
 * Factory method used to instantiate the chosen API backend.
 */
std::unique_ptr<DynamicRHI> createRHI(const RHICreateInfo& rhiCreateInfo);

rhi_END_NAMESPACE;