#pragma once
#ifndef HARPY_SWAPCHAIN
#define HARPY_SWAPCHAIN
#include <utilities/harpy_little_error.h>

#include <buffers/framebuffer.h>

#include "spinal_cord/vulkan_spinal_cord.h"

namespace harpy::nest
{
    struct swapchain_support_details  {
        std::vector<VkSurfaceFormatKHR> formats;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    class swapchain : public interfaces::IStrong_component
    {
        vulkan_spinal_cord& vulkan_backend;
        render_pass& rend;
        
        VkSwapchainKHR chain{nullptr};
        std::vector<buffers::framebuffer> framebuffs;
        std::vector<VkImageView> image_views;
        std::vector<VkImage> images;

        VkSurfaceFormatKHR surface_format {};
        VkPresentModeKHR present_mode {};
        VkExtent2D extent {};
        swapchain_support_details swapchain_details{};
        
        VkExtent2D choose_swap_extent();

        VkSurfaceFormatKHR choose_swapchain_format ();

        VkPresentModeKHR choose_swap_present_mode();

        void init_image_views();

    public:
        
        swapchain(vulkan_spinal_cord& vulkan_backend, render_pass& rend): vulkan_backend(vulkan_backend), rend(rend){}
        
        VkDevice& get_vk_device() override;

        void init();

        void reinit();


        std::vector<buffers::framebuffer>& get_framebuffers(){return framebuffs;}
        std::vector<VkImageView>& get_image_views(){return image_views;}
        std::vector<VkImage>& get_images(){return images;}

        VkSurfaceFormatKHR& get_vk_surface_format() {return surface_format;}
        VkPresentModeKHR& get_vk_present_mode() {return present_mode;}
        VkExtent2D& get_vk_extent() {return extent;}
        swapchain_support_details& get_vk_swapchain_details(){return swapchain_details;}
        VkSwapchainKHR& get_vk_swapchain(){return chain;}
        operator VkSwapchainKHR&(){return chain;}
        render_pass& get_render_pass(){return rend;}

        ~swapchain() override
        {
            for(auto& i : image_views)
            vkDestroyImageView(vulkan_backend.get_vk_device(), i, nullptr);
            
            vkDestroySwapchainKHR(vulkan_backend.get_vk_device(), chain, nullptr);
            
        }
        
    
    };
}
#endif //HARPY_SWAPCHAIN