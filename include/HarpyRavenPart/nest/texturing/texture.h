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

        utilities::images::image* cv_image{nullptr};
        void init();
        void init_view(VkFormat format);
    public:

        texture(
            utilities::images::image& image,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
            VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());
        
        texture(const texture& text) = delete;
        texture(texture&& text) noexcept;
        
        texture& operator=(const texture& text) = delete;
        texture& operator=(texture&& text) noexcept;
        
        VkImage& get_vk_image();
        
        operator VkImage&();
        utilities::images::image* get_raw_cv_image_ptr();
        size_t get_size();
        ~texture();
        
    
    };
}



#endif //HARPY_NEST_TEXTURING_TEXTURE