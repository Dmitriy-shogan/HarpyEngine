#pragma once
#ifndef HARPY_INITIALIZATIONS
#define HARPY_INITIALIZATIONS

#if defined(__WIN32__) || defined(_WIN32)

#define HARPY_OS_LINUX 0
#define HARPY_OS_ANDROID 0
#define HARPY_OS_MACOS 0
#define HARPY_OS_IOS 0
#define HARPY_OS_WINDOWS 1
#define HARPY_OS_NAME "Windows"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32

#elif defined(__APPLE_CC__)

#if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) && __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 40000) || (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED >= 40000)

#define HARPY_OS_LINUX 0
#define HARPY_OS_ANDROID 0
#define HARPY_OS_MACOS 0
#define HARPY_OS_IOS 1
#define HARPY_OS_WINDOWS 0
#define HARPY_OS_NAME "iOS"
#define VK_USE_PLATFORM_IOS_MVK

#else

#define HARPY_OS_LINUX 0
#define HARPY_OS_ANDROID 0
#define HARPY_OS_MACOS 1
#define HARPY_OS_IOS 0
#define HARPY_OS_WINDOWS 0
#define HARPY_OS_NAME "MacOS"
#define VK_USE_PLATFORM_MACOS_MVK
#define GLFW_EXPOSE_NATIVE_COCOA

#endif

#elif defined(__ANDROID__)

#define HARPY_OS_LINUX 0
#define HARPY_OS_ANDROID 1
#define HARPY_OS_MACOS 0
#define HARPY_OS_IOS 0
#define HARPY_OS_WINDOWS 0
#define HARPY_OS_NAME "Android"
#define VK_USE_PLATFORM_ANDROID_KHR

#elif defined(__linux__)

#define HARPY_OS_LINUX 1
#define HARPY_OS_ANDROID 0
#define HARPY_OS_MACOS 0
#define HARPY_OS_IOS 0
#define HARPY_OS_WINDOWS 0
#define HARPY_OS_NAME "Linux"
//JUST FOR NOW, need more research
#define GLFW_EXPOSE_NATIVE_X11
#define VK_USE_PLATFORM_XLIB_KHR

#else
#error Unknown OS
#endif

//Vulkan and volk
#define VK_NO_PROTOTYPES
#include <volk.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

//Some info about engine
#define APP_VERSION VK_MAKE_API_VERSION(0,0,0,1)
#define ENGINE_VERSION VK_MAKE_API_VERSION (0, 0, 0, 1)
#define HARPY_API_VERSION VK_MAKE_API_VERSION(0, 0, 0, 1)
#define ENGINE_NAME "Harpy's engine"

//Max frames that can be rendered at a time
#define MAX_FRAMES_IN_FLIGHT 2

//Glm definitions
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#ifdef HARPY_DEBUG
constexpr bool is_harpy_debug {true};
#else
constexpr bool is_harpy_debug {false};
#endif

#ifdef _DEBUG
constexpr bool is_validation_layers{true};
#else
constexpr bool is_validation_layers{false};
#endif

//TODO: make this into function
#define HARPY_VK_CHECK(x) if (x) throw harpy::utilities::harpy_little_error \
(harpy::utilities::error_severity::wrong_init, "Something went wrong while initializing part of Vulkan")


#define HARPY_MAKE_SINGLETON(class_name)              \
private:                                              \
class_name() = default;                               \
                                                      \
public:                                               \
class_name(class_name const &) = delete;              \
class_name(class_name const &&) = delete;             \
                                                      \
class_name(class_name &) = delete;                    \
class_name(class_name &&) = delete;                   \
                                                      \
class_name& operator =(class_name const &) = delete;  \
class_name& operator =(class_name const &&) = delete; \
                                                      \
class_name& operator =(class_name &) = delete;        \
class_name& operator =(class_name &&) = delete;       \
static class_name& get_singleton(){                   \
static class_name singleton_{};                       \
return singleton_;}                                   \
private:

//Vulkan validation layers debug callback

#include <utilities/logger/logger.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);


namespace harpy
{
    using spirv_compilation_result = std::vector<uint32_t>;
}

//Here goes definitions of most of the things, that needed to be initialized before usage, but couldn't be auto initialized
namespace harpy::nest::resources
{
    class common_vulkan_resource;
}

namespace harpy::nest::windowing{
    struct window_create_info;
}


namespace harpy::nest::initializations
{
    enum class initializations_types
    {
        he_first = 0,
        back = 1 << 0,


        all = back,
        
        he_is_bm = 1,
        he_count = 1,
        
    };

    enum class vendors_id
    {
        he_first = 0x13B5,

        Arm = 0x13B5,
        Nvidia = 0x10DE,
        AMD = 0x1002,
        AMD_2 = 0x1022,
        Intel = 0x8086,
        Qualcom = 0x5143,

        Unkown = 0,


        he_is_bm = 0,
        he_count = 6,
    };

    inline vendors_id vendor;
    
    void init_vk_common_resource(windowing::window_create_info ci);
    void init_harpy(windowing::window_create_info ci);
    void init_std_surface_capabilities();
    void glfw_error_callback(int code, const char* description);
}

#endif