#include "VulkanDevice.hpp"

fmt::color getVendorColor(const uint32_t vendorID)
{
    if (vendorID == 0x1002) return fmt::color::crimson;
    if (vendorID == 0x10DE) return fmt::color::green_yellow;
    if (vendorID == 0x8086) return fmt::color::cornflower_blue;

    return fmt::color::medium_purple;
}

VulkanDevice::VulkanDevice(const VulkanDeviceCreateInfo& createInfo)
: mInstance(createInfo.instance)
{
    selectPhysicalDevice();
    VK_PRINTLN(fmt::format("Using PhysicalDevice: {}", styled(mDeviceName, fg(getVendorColor(mPhysicalDeviceProperties.vendorID)))));

    createDevice();
    VK_VERBOSE("Created Device");
}

std::unique_ptr<VulkanDevice> VulkanDevice::createVulkanDevice(const VulkanDeviceCreateInfo& createInfo)
{
    return std::make_unique<VulkanDevice>(createInfo);
}

void VulkanDevice::waitIdle() const
{
    mDevice.waitIdle();
}

VulkanAllocation* VulkanDevice::allocateMemory(const VulkanAllocationInfo& allocationInfo)
{
    vk::MemoryRequirements memoryRequirements;
    if(std::holds_alternative<vk::Buffer>(allocationInfo.target))
    {
        const auto buffer = std::get<vk::Buffer>(allocationInfo.target);
        memoryRequirements = mDevice.getBufferMemoryRequirements(buffer);
    }
    if(std::holds_alternative<vk::Image>(allocationInfo.target))
    {
        const auto image = std::get<vk::Image>(allocationInfo.target);
        memoryRequirements = mDevice.getImageMemoryRequirements(image);
    }

    constexpr auto allocateFlags = vk::MemoryAllocateFlagsInfo().setFlags(vk::MemoryAllocateFlagBits::eDeviceAddress);
    const auto allocateInfo = vk::MemoryAllocateInfo()
        .setAllocationSize(memoryRequirements.size)
        .setMemoryTypeIndex(findMemoryHeapIndex(memoryRequirements.memoryTypeBits, allocationInfo.propertyFlags))
        .setPNext(&allocateFlags);

    vk::DeviceMemory memory;
    VK_CHECK_DEBUG(
        memory = mDevice.allocateMemory(allocateInfo);
    );

    mMemoryAllocations.push_back(VulkanAllocation::createVulkanAllocation({
        .device        = mDevice,
        .memory        = memory,
        .size          = memoryRequirements.size,
        .target        = allocationInfo.target,
    }));

    return mMemoryAllocations.back().get();
}

void VulkanDevice::selectPhysicalDevice()
{
    const auto requestedExtensions = VulkanDeviceExtension::getRHIDeviceExtensions();

    const auto physicalDevices = mInstance.enumeratePhysicalDevices();
    const auto candidate = std::ranges::find_if(physicalDevices, [&](const vk::PhysicalDevice& physicalDevice) {
        const vk::PhysicalDeviceProperties props = physicalDevice.getProperties();

        const auto supportedExtensions = VulkanDeviceExtension::getDriverDeviceExtensions(physicalDevice);
        bool requirementsPassed = true;

        #ifndef __APPLE__
        if (props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
        {
            return false;
        }
        #endif

        for (const auto& extension : requestedExtensions)
        {
            if (!findExtension(extension->extensionName(), supportedExtensions) and extension->isRequested())
            {
                requirementsPassed = false;
            }
        }

        return requirementsPassed;
    });

    if (candidate == std::end(physicalDevices))
    {
        throw std::runtime_error("Failed to find a suitable PhysicalDevice");
    }

    mPhysicalDevice = *candidate;
    mPhysicalDeviceProperties = mPhysicalDevice.getProperties();
    mDeviceName = std::string(mPhysicalDeviceProperties.deviceName.data());
}

void VulkanDevice::createDevice()
{
    #pragma region "Extensions"

    mDeviceExtensions = VulkanDeviceExtension::getEvaluatedRHIDeviceExtensions(mPhysicalDevice);
    for (auto& extension : mDeviceExtensions)
    {
        if (extension->shouldActivate())
        {
            extension->postSupportCheck();
            if (extension->extensionName() != nullptr)
            {
                mDeviceExtensionNames.push_back(extension->extensionName());
            }
        }
    }

    #pragma endregion

    #pragma region "Queues"

    std::set<uint32_t> uniqueQueueFamilies;

    const auto queueGraphics = findQueue(vk::QueueFlagBits::eGraphics);
    if (queueGraphics.has_value())
    {
        uniqueQueueFamilies.insert(queueGraphics->queueFamilyIndex);
    }

    constexpr float queuePriority = 1.0f;

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    for (const uint32_t familyIndex : uniqueQueueFamilies)
    {
        const auto queueCreateInfo = vk::DeviceQueueCreateInfo()
            .setQueueFamilyIndex(familyIndex)
            .setQueueCount(1)
            .setPQueuePriorities(&queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }

    #pragma endregion

    const auto deviceFeatures = getBaseDeviceFeatures();

    auto createInfo = vk::DeviceCreateInfo()
        .setEnabledExtensionCount(mDeviceExtensionNames.size())
        .setPpEnabledExtensionNames(mDeviceExtensionNames.data())
        .setQueueCreateInfoCount(queueCreateInfos.size())
        .setPQueueCreateInfos(queueCreateInfos.data())
        .setPEnabledFeatures(&deviceFeatures);

    for (const auto& extensions : mDeviceExtensions)
    {
        extensions->preCreateDevice(createInfo);
    }

    VK_EX_CHECK(mDevice = mPhysicalDevice.createDevice(createInfo););

    mGraphicsCommandQueue = VulkanCommandQueue::createVulkanCommandQueue({
        .device                = mDevice,
        .commandBufferCount    = 2,
        .queueFamilyProperties = queueGraphics->queueFamilyProperties,
        .queueFamilyIndex      = queueGraphics->queueFamilyIndex,
        .debugName             = "Graphics"
    });
}

std::optional<VulkanQueueProperties> VulkanDevice::findQueue(vk::QueueFlags requiredFlags, vk::QueueFlags excludedFlags) const
{
    const std::vector<vk::QueueFamilyProperties> queueFamilies = mPhysicalDevice.getQueueFamilyProperties();
    const auto it = std::ranges::find_if(queueFamilies,[requiredFlags, excludedFlags](const vk::QueueFamilyProperties& properties){
        return (properties.queueCount > 0)
           && (properties.queueFlags & requiredFlags)
           && !(properties.queueFlags & excludedFlags);
    });

    if (it == std::end(queueFamilies))
    {
        return std::nullopt;
    }

    const uint32_t familyIndex = static_cast<uint32_t>((it - std::begin(queueFamilies)));
    VulkanQueueProperties queueProperties = { *it, familyIndex };
    return std::make_optional(queueProperties);
}

uint32_t VulkanDevice::findMemoryHeapIndex(uint32_t filter, vk::MemoryPropertyFlags propertyFlags) const
{
    const auto memoryProperties = mPhysicalDevice.getMemoryProperties();
    for (auto i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((filter & (1 << i)) and (memoryProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
        {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory heap");
}

vk::PhysicalDeviceFeatures VulkanDevice::getBaseDeviceFeatures()
{
    return vk::PhysicalDeviceFeatures()
        .setGeometryShader(true)
        .setTessellationShader(true)
        .setFillModeNonSolid(true)
        .setSamplerAnisotropy(true)
        .setSampleRateShading(true)
        .setShaderInt64(true);
}
