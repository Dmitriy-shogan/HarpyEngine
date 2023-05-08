﻿#pragma once
#ifndef HARPY_BUFFERS_INDEX
#define HARPY_BUFFERS_INDEX
#include <buffers/staging_buffer.h>
namespace harpy::nest::buffers
{
    class index_buffer : public base_buffer 
    {
        std::vector<int> indices{
            0, 1, 2, 2, 3, 0
        };
        
    public:
        index_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
            : base_buffer(pool, cord) {}

        void init() override
        {
            buffer_size = sizeof(indices[0]) * indices.size();
     
            create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
     
            staging_buffer stage_buf{pool, vulkan_backend};
            stage_buf.init(buffer_size);
            stage_buf.copy_into_buffer(*this);
        }

        void set_indices(std::vector<int> indices)
        {
            this->indices = std::move(indices);
        }

        std::vector<int> get_indices(){return indices;}
        
    };
}


#endif //HARPY_BUFFERS_INDEX