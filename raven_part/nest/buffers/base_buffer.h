#pragma once
#ifndef HARPY_BASE_BUFFER
#define HARPY_BASE_BUFFER
#include <utilities/harpy_little_error.h>
#include <utilities/utilities.h>
#include <pools/command_pool.h>
#include <queues/transfer_queue.h>

namespace harpy::nest::buffers{
    
    class base_buffer
    {
    protected:
        VkBuffer buffer{};
        VkDeviceMemory buffer_mem{};
        long buffer_size{};
        
        pools::command_pool& pool;
        //TEMPORAL CORD HERE, NEED TO BE CHANGED AFTER QUEUES
        vulkan_spinal_cord& vulkan_backend;
        queues::transfer_queue* queue;
        
        virtual void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        virtual void copy_buffer(VkBuffer& dest, VkCommandPool& pool);

    public:
        base_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord, queues::transfer_queue* queue)
        : pool(pool), vulkan_backend(cord), queue(queue) {}

        virtual void init() = 0;

        virtual ~base_buffer();

        virtual VkBuffer& get_vk_buffer();
        virtual VkDeviceMemory& get_vk_buffer_memory();

        virtual operator VkBuffer&();
    };
}
#endif