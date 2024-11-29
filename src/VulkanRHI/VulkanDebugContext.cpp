#include "VulkanDebugContext.hpp"


VulkanDebugContext::VulkanDebugContext(const VulkanDebugContextCreateInfo& createInfo)
: mInstance(createInfo.instance)
{
    using Severity = vk::DebugUtilsMessageSeverityFlagBitsEXT;
    const auto severity = Severity::eInfo | Severity::eWarning | Severity::eError | Severity::eVerbose;

    using Type = vk::DebugUtilsMessageTypeFlagBitsEXT;
    const auto type = Type::eGeneral | Type::ePerformance | Type::eValidation;

    const auto messengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT()
        .setMessageSeverity(severity)
        .setMessageType(type)
        .setPfnUserCallback(debugMessageCallback);

    VK_CHECK(mMessenger = mInstance.createDebugUtilsMessengerEXT(messengerCreateInfo););
    VK_VERBOSE("DebugUtilsMessenger initialized");
}

std::unique_ptr<VulkanDebugContext> VulkanDebugContext::createVulkanDebugContext(const VulkanDebugContextCreateInfo& createInfo)
{
    return std::make_unique<VulkanDebugContext>(createInfo);
}

VulkanDebugContext::~VulkanDebugContext()
{
    mInstance.destroyDebugUtilsMessengerEXT(mMessenger);
}

VkBool32 VulkanDebugContext::debugMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* p_data, void* p_user)
{
    if (!p_data) return vk::False;
    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        fmt::println("{}", p_data->pMessage);
    }
    return vk::False;
}
