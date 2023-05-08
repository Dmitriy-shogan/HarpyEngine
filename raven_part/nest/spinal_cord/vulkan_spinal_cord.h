#pragma once
#ifndef HARPY_HARD_VULKAN
#define HARPY_HARD_VULKAN

#include <utilities/harpy_little_error.h>
#include "validation_layers.h"
#include <windowing/base_window_layout.h>


namespace harpy::nest {


    //TODO: remove std::runtime_exception and use harpy_little_error exceptions
   
    
    class vulkan_spinal_cord
    {
    protected:
        enum needed_queues_bits
        {
            skip_bit = -1,
            error_bit = 0,
            graphic_bit = 1,
            present_bit,
        };
        struct needed_queues_families
        {
            //Graphic family for render itself
            std::optional<uint32_t> graphics_families;

            //Graphic family for displaying
            std::optional<uint32_t> present_families;

            bool is_completed() const
            {
                return graphics_families.has_value() && present_families.has_value();
            }
        };
        
        

        const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkInstance instance = nullptr;
        VkPhysicalDevice ph_device = nullptr;
        VkDevice device = nullptr;
        VkQueue graphics_queue = nullptr;
        VkQueue present_queue = nullptr;

        validation_layers base_valid{&instance};

        windowing::base_window_layout& connected_window_layout;
        
        
        void init_ph_device();
        void init_device_and_queues();
        void init_debug();
    public:
        vulkan_spinal_cord(windowing::base_window_layout& window_layout) : connected_window_layout(window_layout){} 
        

        void init_instance();
        void init()
        {
            init_debug();
            init_ph_device();
            init_device_and_queues();
        }

        //for later
        static bool is_device_suitable(VkPhysicalDevice phys_device);

        //Bits isn't realised yet
        static needed_queues_families find_queue_families(VkPhysicalDevice& ph_device, VkSurfaceKHR& surface);

        bool check_device_extension_support() const;
        void connect_window(windowing::base_window_layout& win, bool do_init);
        static std::vector<const char*> get_required_extensions();
        VkDevice& get_vk_device(){return device;}
        VkPhysicalDevice& get_vk_physical_device(){return ph_device;}
        VkInstance& get_vk_instance(){return instance;}
        VkSurfaceKHR& get_vk_surface(){return connected_window_layout.get_VK_surface();}
        VkQueue& get_vk_present_queue(){return present_queue;}
        VkQueue& get_vk_graphics_queue(){return graphics_queue;}
        windowing::base_window_layout& get_window_layout(){return connected_window_layout;}

        virtual ~vulkan_spinal_cord();
    };
    
}

#endif