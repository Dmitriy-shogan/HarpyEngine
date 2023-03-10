#pragma once
#ifndef HARPY_HARD_VULKAN
#define HARPY_HARD_VULKAN

#include "..//utilities/harpy_little_error.h"
#include "hard/validation_layers.hpp"
#include "..//vision/base_window_layout.hpp"


namespace harpy_nest {


    //TODO: remove std::runtime_exception and use harpy_little_error exceptions
   
    
    class hard_level_vulkan
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

        base_window_layout* connected_window_layout{nullptr};

        hard_level_vulkan() = default;

        
        void init_ph_device(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);
        void init_device_and_queues(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);

        void init_default_hard();

        void init_debug();
    public:

        void init_instance(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);

        //for later
        static bool is_device_suitable(VkPhysicalDevice phys_device);

        //Bits isn't realised yet
        needed_queues_families find_queue_families(needed_queues_bits bits = skip_bit) const;

        bool check_device_extension_support() const;
        void connect_window(base_window_layout& win, bool do_init);
        static std::vector<const char*> get_required_extensions();

        virtual ~hard_level_vulkan();
    };
    
}

#endif