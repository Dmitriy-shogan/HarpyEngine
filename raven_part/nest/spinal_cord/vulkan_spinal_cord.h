#pragma once
#ifndef HARPY_HARD_VULKAN
#define HARPY_HARD_VULKAN

#include <utilities/harpy_little_error.h>
#include "validation_layers.h"
#include <windowing/base_window_layout.h>


namespace harpy::nest {

    //TODO: make transfer-only queue, if exists one
    //TODO: remove std::runtime_exception and use harpy_little_error exceptions
   
    
    class vulkan_spinal_cord
    {
        const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkInstance instance = nullptr;
        VkPhysicalDevice ph_device = nullptr;
        VkDevice device = nullptr;
        validation_layers base_valid{&instance};
        needed_queues_families indices;

        windowing::base_window_layout& connected_window_layout;
        
        
        void init_ph_device();
        void init_devices();
        void init_debug();
    public:
        vulkan_spinal_cord(windowing::base_window_layout& window_layout) : connected_window_layout(window_layout){} 
        

        void init_instance();
        void init()
        {
            init_debug();
            init_ph_device();
            init_devices();
        }

        //for later
        static bool is_device_suitable(VkPhysicalDevice phys_device);

        
        bool check_device_extension_support() const;
        void connect_window(windowing::base_window_layout& win, bool do_init);
        static std::vector<const char*> get_required_extensions();
        VkDevice& get_vk_device(){return device;}
        VkPhysicalDevice& get_vk_physical_device(){return ph_device;}
        VkInstance& get_vk_instance(){return instance;}
        VkSurfaceKHR& get_vk_surface(){return connected_window_layout.get_VK_surface();}
        windowing::base_window_layout& get_window_layout(){return connected_window_layout;}
        needed_queues_families get_queue_families(){return indices;}

        virtual ~vulkan_spinal_cord();
    };
    
}

#endif