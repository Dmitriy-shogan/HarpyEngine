#include "validation_layers.hpp"


harpy_nest::validation_layers::validation_layers(VkInstance* instance): connected_instance(instance) {}

void harpy_nest::validation_layers::init_debug_messenger()
{
    if constexpr  (VALIDATION_LAYERS) throw std::runtime_error("No debug information without validation layers");

    VkDebugUtilsMessengerCreateInfoEXT create_info_ext;
    basic_debug_init(create_info_ext);

    if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>
        (vkGetInstanceProcAddr(*connected_instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr) {
        func(*connected_instance, &create_info_ext, nullptr, &debug_messenger);
    } else {
        throw std::exception(("Failed to set up debug messenger at " + std::to_string(__LINE__) + " line").c_str());
    }
}

void harpy_nest::validation_layers::basic_debug_init(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = basic_debug_callback;
}

VkBool32 harpy_nest::validation_layers::basic_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                             void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

harpy_nest::validation_layers::~validation_layers()
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(*connected_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(*connected_instance, debug_messenger, nullptr);
    }
}
