#pragma once
#ifndef HARPY_NEST_TEXTURING_TEXTURE
#define HARPY_NEST_TEXTURING_TEXTURE
#include <nest/resources/common_vulkan_resource.h>
#include <images/image.h>


namespace harpy::nest::texturing
{
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


        void init_view(VkFormat format);
    public:

        texture(
            utilities::image& image,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
            VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        texture(
                VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
                VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        void init(utilities::image& image);
        
        texture(const texture& text) = delete;
        texture(texture&& text) noexcept;
        
        texture& operator=(const texture& text) = delete;
        texture& operator=(texture&& text) noexcept;
        
        VkImage& get_vk_image();
        operator VkImage&();

        VkImageView& get_vk_image_view();
        operator VkImageView&();

        size_t get_size();
        ~texture();
    };
}



#endif //HARPY_NEST_TEXTURING_TEXTURE