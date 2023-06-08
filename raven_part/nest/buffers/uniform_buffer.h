#pragma once
#ifndef HARPY_BUFFERS_UNIFORM
#define HARPY_BUFFERS_UNIFORM
#include <buffers/base_buffer.h>


namespace harpy::nest::buffers
{
    class uniform_buffer : public base_buffer
    {
        void* data;
    public:
        uniform_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
            : base_buffer(pool, cord)
        {
            buffer_size = sizeof(ubo);
        }

        void init() override
        {
            create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            vkMapMemory(pool.get_vk_device(), buffer_mem, 0, buffer_size, 0, &data);
        }

        void set_ubo(ubo unif)
        {
            memcpy(data, &unif, sizeof(ubo));
        }

        void* get_data(){return data;}
        
    };
}

#endif //HARPY_BUFFERS_UNIFORM