#pragma once
#ifndef HARPY_VALIDATION_LAYERS
#define HARPY_VALIDATION_LAYERS
#include "..//..//utilities/initializations.hpp"


namespace harpy_nest {
class validation_layers
{
    VkInstance* connected_instance;
    VkDebugUtilsMessengerEXT debug_messenger{};
    
public:
    const std::vector<const char*> layers{ "VK_LAYER_KHRONOS_validation" };
    

    validation_layers(VkInstance* instance);

    void init_debug_messenger();

    static void basic_debug_init(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL basic_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    ~validation_layers();
};

}


#endif