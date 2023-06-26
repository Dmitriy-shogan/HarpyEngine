#pragma once
#ifndef HARPY_QUEUE_BASE
#define HARPY_QUEUE_BASE
#include <spinal_cord/vulkan_spinal_cord.h>

/*
 * Temporal solution: we are currently using only one queue at all. 
 * Later, after сессия, we need algorithm to summarize all queues and chooses
 * what queues will be graphical, transfer or present
 */

namespace harpy::nest::queues
{
    
    class base_queue
    {
    protected:
        vulkan_spinal_cord& cord;
        VkQueue queue {nullptr};
        uint32_t queue_family_index{};
        
    public:
        base_queue(vulkan_spinal_cord& cord) : cord(cord){}
        virtual void init() = 0;
        virtual ~base_queue() = default;
        virtual VkQueue& get_vk_queue();
        virtual uint32_t get_family_index();
    };
}

#endif //HARPY_QUEUE