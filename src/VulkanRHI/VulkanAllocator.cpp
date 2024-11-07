#include "VulkanAllocator.hpp"

uint32_t VulkanAllocation::sSequence = 0;

VulkanAllocation::VulkanAllocation(const VulkanAllocationCreateInfo& createInfo)
: mMemory(createInfo.memory)
, mSize(createInfo.size)
, mUser(createInfo.target)
, mDevice(createInfo.device)
, mId(sSequence++)
{
}

std::unique_ptr<VulkanAllocation> VulkanAllocation::createVulkanAllocation(const VulkanAllocationCreateInfo& createInfo)
{
    return std::make_unique<VulkanAllocation>(createInfo);
}

VulkanAllocation::~VulkanAllocation()
{
    free();
}

void VulkanAllocation::bind()
{
    if (std::holds_alternative<vk::Buffer>(mUser))
    {
        auto& buffer = std::get<vk::Buffer>(mUser);
        mDevice.bindBufferMemory(buffer, mMemory, mOffset);

        if (!mAddress.has_value())
        {
            const auto deviceAddressInfo = vk::BufferDeviceAddressInfo()
                .setBuffer(buffer);
            VK_CHECK_DEBUG(mAddress = mDevice.getBufferAddress(deviceAddressInfo););
        }
    }
    if (std::holds_alternative<vk::Image>(mUser))
    {
        auto& image = std::get<vk::Image>(mUser);
        mDevice.bindImageMemory(image, mMemory, mOffset);
    }

    mIsBound = true;
}

void* VulkanAllocation::map()
{
    void* mapped_memory = nullptr;
    VK_CHECK_DEBUG(mapped_memory = mDevice.mapMemory(mMemory, mOffset, mSize, {}););

    mIsMapped = true;
    return mapped_memory;
}

void VulkanAllocation::unmap()
{
    if (mIsMapped)
    {
        mDevice.unmapMemory(mMemory);
        mIsMapped = false;
    }
}

void VulkanAllocation::free()
{
    if (!mInvalid)
    {
        mDevice.free(mMemory);
        mInvalid = true;
    }
}

std::optional<vk::DeviceAddress> VulkanAllocation::getAddress() const
{
    #ifdef VULKAN_DEBUGGING_ENABLED
    if (!mIsBound and std::holds_alternative<vk::Buffer>(mUser))
    {
        VK_DEBUG(fmt::format("{}", styled("Tried to get DeviceAddress for Buffer DeviceMemory, but the Buffer memory hasn't been bound to it yet!", fg(fmt::color::light_yellow))));
    }
    #endif

    return mAddress;
}
