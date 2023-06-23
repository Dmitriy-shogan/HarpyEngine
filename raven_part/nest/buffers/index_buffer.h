#pragma once
#ifndef HARPY_BUFFERS_INDEX
#define HARPY_BUFFERS_INDEX
#include <buffers/base_buffer.h>
namespace harpy::nest::buffers
{
    class index_buffer : public base_buffer 
    {
        std::vector<uint32_t> indices{
            0, 1, 2, 2, 3, 0
        };
        
    public:
        index_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
            : base_buffer(pool, cord) {}

        void init() override
        {
            buffer_size = sizeof(indices[0]) * indices.size();
            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            create_buffer(vulkan_backend.get_vk_device(),
                vulkan_backend.get_vk_physical_device(),
                buffer_size,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                stagingBuffer,
                stagingBufferMemory);

            void* data;
            vkMapMemory(vulkan_backend.get_vk_device(), stagingBufferMemory, 0, buffer_size, 0, &data);
            memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
            vkUnmapMemory(vulkan_backend.get_vk_device(), stagingBufferMemory);
            
            create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            utilities::vk_copy_buffer(stagingBuffer, buffer, buffer_size, vulkan_backend.get_vk_device(), pool, vulkan_backend.get_vk_graphics_queue());

            vkDestroyBuffer(vulkan_backend.get_vk_device(), stagingBuffer, nullptr);
            vkFreeMemory(vulkan_backend.get_vk_device(), stagingBufferMemory, nullptr);
            
        }

        void set_indices(std::vector<uint32_t> indices)
        {
            this->indices = std::move(indices);
        }

        std::vector<uint32_t> get_indices(){return indices;}
        
    };
}


#endif //HARPY_BUFFERS_INDEX