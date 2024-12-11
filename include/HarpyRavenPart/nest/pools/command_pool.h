#pragma once
#ifndef HARPY_NEST_POOLS_COMMAND
#define HARPY_NEST_POOLS_COMMAND
#include "nest/wrappers/queue_family.h"
#include <nest/resources/buffer_set_resource.h>

namespace harpy::nest::pools
{
    class command_pool
    {
        resources::buffer_set_resource buffer_set{};
        VkCommandPool pool{};
        VkDevice* device{};

    public:

        command_pool(wrappers::queue_family& family_indice, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        command_pool(uint32_t family_indice, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        command_pool(const command_pool&) = delete;
        command_pool& operator=(const command_pool&) = delete;

        command_pool(command_pool&&);
        command_pool& operator=(command_pool&&);

        void allocate_buffers(bool is_primary, int amount);

        resources::buffer_set_resource& get_buffer_set();
        std::vector<VkCommandBuffer>& get_primary_buffers();
        std::unordered_map<VkCommandBuffer, std::size_t>& get_secondary_buffers();
        
        void reset_pool();
        void trim_pool();

        VkCommandPool& get_vk_pool();
        operator VkCommandPool&();
        
        ~command_pool();
    };
}
#endif //HARPY_NEST_POOLS_COMMAND