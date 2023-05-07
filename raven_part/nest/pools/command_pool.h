#pragma once
#ifndef HARPY_COMMAND_POOL_POOL
#define HARPY_COMMAND_POOL_POOL
#include <utilities/harpy_little_error.h>
#include <interfaces/interfaces.h>


namespace harpy::nest::pools {

    enum command_pool_types
    {
        draw = 1,
        copy = 1 << 1,
    };
    
class command_pool : interfaces::IStrong_component
{
    VkDevice& device;
    command_pool_types type;

    needed_queues_families queue_families;
    VkCommandPool pool{nullptr};

    void init_draw_pool();
    void init_copy_pool();
public:
    command_pool(VkDevice& device, needed_queues_families queue_families, command_pool_types type = draw)
    : device(device), type(type), queue_families(queue_families) {}

    void init()
    {
        switch(type)
        {
        case draw:
            {
                init_draw_pool();
                break;
            }
        case copy:
            {
                init_copy_pool();
                break;
            }
        }
    }

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