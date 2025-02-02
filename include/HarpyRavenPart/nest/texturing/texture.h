#pragma once
#ifndef HARPY_NEST_TEXTURING_TEXTURE
#define HARPY_NEST_TEXTURING_TEXTURE
#include <nest/resources/common_vulkan_resource.h>
#include <images/image.h>


namespace harpy::nest::texturing
{
    enum texture_type {
        default_type = 1,
        specular = 2,
        diffuse = 4,
        normal = 8,
        height = 16,
        ambient = 32,
        emissive = 64,
    };
    class texture
    {
        VkDevice* device{};
        VmaAllocator* allocator{};
        
        VkImage image{};
        VmaAllocation allocation{};
        VkImageView view{};
        VkExtent3D extent{};
        VkFormat format{};
        size_t image_size{};

        uint32_t height{};
        uint32_t width{};


        sz::string id{"default"};

        void init_view(VkFormat format);
    public:

        texture(
            sz::string_view id,
            utilities::image& image,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
            VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        texture(
            sz::string_view id,
                VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
                VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        void init(utilities::image& image);
        void init(void* data, uint32_t width, uint32_t height);
        
        texture(const texture& text) = delete;
        texture& operator=(const texture& text) = delete;

        texture(texture&& text) noexcept;
        texture& operator=(texture&& text) noexcept;
        
        VkImage& get_vk_image();
        operator VkImage&();

        VkImageView& get_vk_image_view();
        operator VkImageView&();

        size_t get_size();

        sz::string get_id();
        void set_id(sz::string_view id);

        ~texture();
    };
}



#endif //HARPY_NEST_TEXTURING_TEXTURE