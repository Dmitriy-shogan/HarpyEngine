#pragma once
#ifndef HARPY_BUFFERS_STAGING
#define HARPY_BUFFERS_STAGING
#include <buffers/base_buffer.h>
#include <pools/command_pool.h>

namespace harpy::nest::buffers
{
    class vertex_buffer;
    class index_buffer;
    class staging_buffer : public base_buffer
    {
        
    public:
        staging_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
            : base_buffer(pool, cord)
        {
        }

        /**
         * \brief Init is empty in staging buffer cause you need to specify buffer size. Don't use it
         */
        void init() override {}
        
        void init(int buffer_size) 
        {
            create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        
        void copy_into_buffer(vertex_buffer& buff);
        void copy_into_buffer(index_buffer& buff);
    };
}
#endif //HARPY_BUFFERS_STAGING