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
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
        };

        
        void init_vertex_buffer();

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const;


    public:
        vertex_buffer(soft_level_vulkan* vulkan_back) :vulkan_backend(vulkan_back) {}

        void init_buffer(std::vector<vertex> vertices);

        void init_standard_buffer();

        VkBuffer& get_vk_buffer();
        uint32_t get_vertices_size() const;


        ~vertex_buffer();
    };

}

#endif //HARPY_VERTEX_BUFFER