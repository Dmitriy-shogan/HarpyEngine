#pragma once
#ifndef HARPY_SWAPCHAIN
#define HARPY_SWAPCHAIN
#include <utilities/harpy_little_error.h>



#include <optional>
#include <vector>

namespace harpy::nest
{
	struct vulkan_spinal_cord;
	struct renderer_context;

    struct swapchain_support_details  {
        std::vector<VkSurfaceFormatKHR> formats{};
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    struct swapchain
    {
    	renderer_context * r_context;
        VkSwapchainKHR chain = nullptr;

        std::vector<VkImageView> image_views;
        std::vector<VkImage> images;

        VkSurfaceFormatKHR surface_format{};
        VkPresentModeKHR present_mode{};
        VkExtent2D extent{};
        swapchain_support_details swapchain_details{};
        
        VkExtent2D choose_swap_extent();
        VkSurfaceFormatKHR choose_swapchain_format ();
        VkPresentModeKHR choose_swap_present_mode();


        void init_image_views();
        
    public:
        
        swapchain(renderer_context * r_context);
        
        void init();
        void reinit();

        VkDevice& get_vk_device();

        




        std::vector<VkImageView>& get_image_views(){return image_views;}
        std::vector<VkImage>& get_images(){return images;}

        VkSurfaceFormatKHR& get_vk_surface_format() {return surface_format;}
        VkPresentModeKHR& get_vk_present_mode() {return present_mode;}
        VkExtent2D& get_vk_extent() {return extent;}
        swapchain_support_details& get_vk_swapchain_details(){return swapchain_details;}
        VkSwapchainKHR& get_vk_swapchain(){return chain;}
        operator VkSwapchainKHR&(){return chain;}

        ~swapchain();
        
    
    };
}
#endif //HARPY_SWAPCHAIN
