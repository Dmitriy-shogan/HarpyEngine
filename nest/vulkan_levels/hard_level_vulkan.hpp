#pragma once
#ifndef HARPY_HARD_VULKAN
#define HARPY_HARD_VULKAN

#include "..//utilities/initializations.hpp"
#include "hard/validation_layers.hpp"
#include "..//vision/base_window_layout.hpp"

namespace harpy_nest {

    
    
    class hard_level_vulkan
    {
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

        bool is_inited = false;
        
        VkInstance instance = nullptr;
        VkPhysicalDevice ph_device = nullptr;
        VkDevice device = nullptr;
        VkQueue graphics_queue = nullptr;
        VkQueue present_queue = nullptr;

        validation_layers base_valid{&instance};

        base_window_layout* connected_window_layout{nullptr};

        

    public:

        //for later
        bool is_device_suitable() const;

        //Bits isn't realised yet
        [[nodiscard]] needed_queues_families find_queue_families(needed_queues_bits bits = skip_bit) const;

        [[nodiscard]] bool check_device_extension_support() const;
        
        hard_level_vulkan() = default;

        static std::vector<const char*> get_required_extensions();

        void init_instance(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);
        void init_ph_device(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);
        void init_device_and_queues(harpy_hard_level_settings settings = harpy_hard_level_settings::standard);

        void init_all_default();

        void init_debug();

        void connect_window(base_window_layout& win, bool do_init);
        
        VkInstance get_instance() const;
        VkPhysicalDevice get_ph_device() const;
        VkDevice get_device() const;
        VkQueue get_graphics_queue() const;
        VkQueue get_present_queue() const;
        validation_layers& get_valid_layers();
        base_window_layout* get_base_window_layout() const;

        explicit operator bool() const;

        const std::vector<const char*>& get_device_extensions() const;

        ~hard_level_vulkan();
    };
    
}

#endif