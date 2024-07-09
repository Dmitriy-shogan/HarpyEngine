#pragma once
#ifndef HARPY_NEST_BUFFERS_DATA
#define HARPY_NEST_BUFFERS_DATA
#include <glm/glm.hpp>
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::wrappers
{
    
    struct vertex
    {
        //TODO: don't forget about vertex structure
        glm::vec3 coords{};
        glm::vec3 base_color{};
    };

    //JUST FOR NOW
    inline VkVertexInputBindingDescription vertex_vk_description{
        .binding = 0,
        .stride = sizeof(vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    inline std::vector<VkVertexInputAttributeDescription> vertex_vk_input_attribute_descriptions{
                    {
                        .location = 0,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(vertex, coords),
                    },
                    {
                        .location = 1,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset = offsetof(vertex, base_color),
                    }
    };
    
    enum class buffer_type
    {
        vertex = 0,
        indice,
        staging,
        uniform,
        storage,

        he_first = vertex,
        he_is_bm = 0,
        he_sum = 5
    };
    
    class data_buffer
    {
        VmaAllocator* allocator{};
        VmaAllocation alloc{};
        VkDevice* device{};
        std::size_t size{};
        buffer_type type;
        VkBuffer buffer{};
        
        
    public:
        
        data_buffer(buffer_type type, std::size_t size = 0,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
            VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());
        
        void init(std::size_t size);
        
        data_buffer(const data_buffer& buffer) = delete;
        data_buffer& operator=(const data_buffer& buffer) = delete;
        
        data_buffer(data_buffer&& buffer) noexcept;
        data_buffer& operator= (data_buffer&& buffer) noexcept;

        VkBuffer& get_vk_buffer();
        operator VkBuffer&();

        const VmaAllocation& get_vma_allocation();
        operator VmaAllocation&();

        const uint32_t get_size();

        const buffer_type get_type();
        operator buffer_type const();

        VkDevice*& get_vk_device();
        VmaAllocator*& get_vk_allocator();

        void* get_mapped_ptr();
        void unmap_ptr();
        
        ~data_buffer();
    };
}




#endif //HARPY_NEST_BUFFERS_DATA