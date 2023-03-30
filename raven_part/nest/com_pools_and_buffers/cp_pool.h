#pragma once
#ifndef HARPY_COMMAND_POOL_POOL
#define HARPY_COMMAND_POOL_POOL
#include <utilities/harpy_little_error.h>


namespace harpy_nest {

    class hard_level_vulkan;
    class soft_level_vulkan;

class cp_pool
{
    
    VkDevice* device{nullptr};
    static bool is_one;

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

    needed_queues_families queue_families;
    
    VkCommandPoolCreateInfo pool_info{};
    std::vector<VkCommandPool> draw_pools{1};
    std::vector<VkCommandPool> copy_pools{1};

public:
    cp_pool(soft_level_vulkan* vulkan_back);


    VkCommandPool& create_draw_pool();

    VkCommandPool& create_copy_pool();

    void generate_copy_pools(int how_many);
    
    void generate_draw_pools(int how_many);

    VkCommandPool& get_copy_pool(int index);
    VkCommandPool& get_first_copy_pool();
    VkCommandPool& get_last_copy_pool();

    VkCommandPool& get_draw_pool(int index);
    VkCommandPool& get_first_draw_pool();
    VkCommandPool& get_last_draw_pool();

    ~cp_pool();
};
    
}


#endif