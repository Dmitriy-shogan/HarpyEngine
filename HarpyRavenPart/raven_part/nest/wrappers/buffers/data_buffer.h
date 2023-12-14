#pragma once
#ifndef HARPY_NEST_BUFFERS_DATA
#define HARPY_NEST_BUFFERS_DATA
#include <glm/glm.hpp>
#include <nest/initializations/inititalizations.h>
#include "nest/resources/common_vulkan_resource.h"

namespace harpy::nest::wrappers
{
    struct vertex
    {
        //TODO: don't forget about vertex structure
        glm::vec3 coords{};
        glm::vec4 base_color{};
    };
    
    enum class buffer_type
    {
        
        vertex = 0,
        indice,

        he_first = vertex,
        he_is_bm = 0,
        he_sum = 2
    };
    
    class data_buffer
    {
    protected:
        VmaAllocator* allocator{};
        VmaAllocation alloc{};
        VkDevice* device{};
        uint32_t size{};
        buffer_type type;
        VkBuffer buffer{};
        
    public:
        data_buffer() = default;

        void init(uint32_t size, buffer_type type, 
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
            VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        data_buffer(data_buffer& buffer) noexcept;
        data_buffer(const data_buffer& buffer) noexcept;
        data_buffer operator=(data_buffer& buffer) noexcept;
        data_buffer operator=(const data_buffer& buffer) noexcept;

        data_buffer(data_buffer&& buffer) noexcept;
        data_buffer operator= (data_buffer&& buffer) noexcept;

        VkBuffer& get_vk_buffer();
        operator VkBuffer&();

        VmaAllocation& get_vma_allocation();
        operator VmaAllocation&();

        uint32_t get_size();

        buffer_type get_type();
        operator buffer_type();
        
        ~data_buffer();
    };
}




#endif //HARPY_NEST_BUFFERS_DATA