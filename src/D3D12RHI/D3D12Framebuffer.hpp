#pragma once

#include "D3D12Core.hpp"
#include "RHI/RHIFramebuffer.hpp"

/**
 * D3D12 has no concept of framebuffers, this class is just an empty impostor to comply with Vulkan's needs.
 */
class D3D12Framebuffer final : public RHIFramebuffer
{
public:
    DISABLE_COPY_CTOR(D3D12Framebuffer);
    DEF_PRIMARY_CTOR(D3D12Framebuffer);
    ~D3D12Framebuffer() override = default;

    // As D3D12 calls won't actually use Framebuffers, returning a nullptr should be fine... in theory...
    RHIFramebufferHandle* getFramebuffer(size_t index) override
    {
        return nullptr;
    }
};
