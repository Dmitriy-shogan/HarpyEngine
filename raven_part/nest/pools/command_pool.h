#pragma once
#ifndef HARPY_POOLS_COMMAND
#define HARPY_POOLS_COMMAND
#include <utilities/harpy_little_error.h>
#include <interfaces/interfaces.h>

#include "queues/transfer_queue.h"


namespace harpy::nest::pools
{
    enum command_pool_types
    {
        draw = 1,
        copy = 1 << 1,
    };

    //TODO: make command pool base and make new class for draw and copy pools
    class command_pool : interfaces::IStrong_component
    {
        VkDevice& device;
        command_pool_types type;

        queues::base_queue* queue{nullptr};
        VkCommandPool pool{nullptr};

        void init_draw_pool();
        void init_copy_pool();
    public:
        command_pool(VkDevice& device, command_pool_types type = draw)
        : device(device), type(type) {}

        void init(queues::transfer_queue* queue);

        VkCommandPool& get_vk_command_pool(){return pool;}
        VkDevice& get_vk_device() override{return device;}
        command_pool_types get_type() {return type;}

        operator VkCommandPool&(){return pool;}
    

        ~command_pool() override
        {
            vkDestroyCommandPool(device, pool, nullptr);
        }
    };
}


#endif