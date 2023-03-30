#pragma once
#ifndef HARPY_INDEX_BUFFER
#define HARPY_INDEX_BUFFER
#include <utilities/harpy_little_error.h>

namespace harpy_nest{

    class soft_level_vulkan;
    
class index_buffer
{
    soft_level_vulkan* vulkan_backend{nullptr};
    std::vector<uint32_t> indices {0, 1, 2, 2, 3, 0};
    VkBuffer index_buf;
    VkDeviceMemory index_mem;
    
    void init_index_buffer();

    uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const;


public:
    index_buffer(soft_level_vulkan* vulkan_back) : vulkan_backend(vulkan_back), index_buf(nullptr), index_mem(nullptr)
    {
    }

    void init_buffer(std::vector<uint32_t> indices);

    void init_standard_buffer();

    VkBuffer& get_vk_buffer();
    uint32_t get_indices_size() const;


    ~index_buffer();
    
};

}
#endif