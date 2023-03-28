#pragma once
#ifndef HARPY_VERTEX_BUFFER
#define HARPY_VERTEX_BUFFER
#include "vulkan_levels/soft_level_vulkan.h"

namespace harpy_nest {
    class vertex_buffer
    {
        soft_level_vulkan* vulkan_backend{nullptr};
        VkBuffer vertex_buf{nullptr};
        VkDeviceMemory vertex_buffer_memory{nullptr};

        std::vector<vertex> vertices {
                    {{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                    {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                    {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
        };
        
        void init_vertex_buffer()
        {
            VkBufferCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            create_info.size = sizeof(vertices[0]) * vertices.size();
            create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if(vkCreateBuffer(vulkan_backend->get_device(), &create_info, nullptr, &vertex_buf))
                throw harpy_little_error(error_severity::wrong_init, "Vertex buffer was not created succesfully");

            VkMemoryRequirements mem_requirements;
            vkGetBufferMemoryRequirements(vulkan_backend->get_device(), vertex_buf, &mem_requirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = mem_requirements.size;
            allocInfo.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            if (vkAllocateMemory(vulkan_backend->get_device(), &allocInfo, nullptr, &vertex_buffer_memory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate vertex buffer memory!");
            }
            
            
        }

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const
        {
            
            VkPhysicalDeviceMemoryProperties mem_properties;
            vkGetPhysicalDeviceMemoryProperties(vulkan_backend->get_ph_device(), &mem_properties);

            for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
                if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) 
                    return i;
            }

            throw harpy_little_error("failed to find suitable memory type!");
            
        }

        
        public:
        vertex_buffer(soft_level_vulkan* vulkan_back) :vulkan_backend(vulkan_back) {}

        void init_buffer(std::vector<vertex>& vertices)
        {
            this->vertices = vertices;
            init_vertex_buffer();
        }

        
        ~vertex_buffer()
        {
            vkDestroyBuffer(vulkan_backend->get_device(), vertex_buf, nullptr);
        }
};

}

#endif //HARPY_VERTEX_BUFFER