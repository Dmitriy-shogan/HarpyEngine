#pragma once
#ifndef HARPY_RESOURCE_VULKAN_COMMON
#define HARPY_RESOURCE_VULKAN_COMMON
#include <vector>
#include "nest/wrappers/queue_family.h"
#include <nest/windowing/window.h>
#include <nest/inititalizations.h>

namespace harpy::nest::resources
{
    const std::vector validation_layers = {
        "VK_LAYER_KHRONOS_validation"
    };
    const std::vector musthave_device_extentions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    const std::vector preferrable_instance_extensions = {
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME
    };

    const std::vector musthave_instance_extentions = {
        "VK_LAYER_KHRONOS_synchronization2"
#if defined(_DEBUG) || defined(HARPY_DEBUG) 
        ,"VK_LAYER_KHRONOS_validation"
#endif
    };

    const std::vector preferrable_device_extentions = {
        VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
    };
    
    /*
     * TODO: make some more operators for not just &, but for pure ptrs of vulkan (*)
     */
    //Singleton
    class common_vulkan_resource
    {
        bool is_instance_initialized{false};
        
        VkInstance instance{};
        std::vector<VkSurfaceKHR> surfaces{};
        VkDebugUtilsMessengerEXT debug_messenger{};
        VkPhysicalDevice phys_device{};
        VkPhysicalDeviceFeatures2 phys_features{};
        VkPhysicalDeviceProperties2 phys_props{};
        std::vector<VkDevice> devices{};

        //Ideally do save vectors and use them here
        std::vector<wrappers::queue_family> family_queues{};
        std::vector<windowing::window> windows{};
        
        //Allocator connected to the device
        std::vector<VmaAllocator> allocators{};
        
        
        common_vulkan_resource() = default;
    public:
        
        common_vulkan_resource(common_vulkan_resource&) = delete;
        common_vulkan_resource& operator=(common_vulkan_resource&) = delete;
        
        common_vulkan_resource(common_vulkan_resource&&) = delete;
        common_vulkan_resource& operator=(common_vulkan_resource&&) = delete;

        VkInstance& get_instance();
        operator VkInstance&();

        std::vector<VkSurfaceKHR>& get_all_surfaces();

        
        VkSurfaceKHR& get_main_surface();
        operator VkSurfaceKHR&();

        VkDebugUtilsMessengerEXT& get_debug_messenger();
        operator VkDebugUtilsMessengerEXT&();

        VkPhysicalDevice& get_phys_device();
        operator VkPhysicalDevice&();
        VkPhysicalDeviceFeatures2& get_phys_features();
        operator VkPhysicalDeviceFeatures2&();
        VkPhysicalDeviceProperties2& get_phys_props();
        operator VkPhysicalDeviceProperties2&();
        
        std::vector<VkDevice>& get_all_devices();
        
        VkDevice& get_main_device();
        operator VkDevice&();
        
        std::vector<windowing::window>& get_all_windows();

        //RETURN MAIN WINDOW
        windowing::window& get_main_window();
        operator windowing::window&();

        void set_main_window(windowing::window_create_info);
        void set_main_window(windowing::window&&);

        std::size_t add_window(windowing::window_create_info);
        std::size_t add_window(windowing::window&&);
        
        std::vector<wrappers::queue_family>& get_all_family_queues();
        wrappers::queue_family& get_main_family_queue();

        std::vector<VmaAllocator>& get_allocators();
        VmaAllocator& get_main_allocator();
        operator VmaAllocator&();

        bool is_inited();


        static common_vulkan_resource& get_resource();
        ~common_vulkan_resource();
    };
}

#endif //HARPY_RESOURCE_VULKAN_COMMON
