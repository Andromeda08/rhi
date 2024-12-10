#include "D3D12Framebuffer.hpp"

D3D12Framebuffer::D3D12Framebuffer()
: RHIFramebuffer()
{
}

std::unique_ptr<D3D12Framebuffer> D3D12Framebuffer::createD3D12Framebuffer()
{
    return std::make_unique<D3D12Framebuffer>();
}
