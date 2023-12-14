#pragma once
#ifndef HARPY_NEST_WRAPPERS_SWAPCHAIN
#define HARPY_NEST_WRAPPERS_SWAPCHAIN
#include <memory>
#include <vector>
#include <nest/wrappers/render_pass/render_pass.h>

namespace harpy::nest::wrappers
{
    struct swapchain_ci;
    
    class swapchain
    {
        VkSwapchainKHR chain{};
        std::vector<VkImageView> views{};
        std::vector<VkImage> images{};
        std::vector<VkFramebuffer> framebuffers{};
        
        VkFormat format{};
        VkColorSpaceKHR color_space{};
        VkPresentModeKHR present_mode{};
        VkExtent2D extent{};

        void init_image_views();
        void init_framebuffers(render_pass pass);
        
        friend class swapchain_manager;
        
    public:

        swapchain() = default;

        void init(std::shared_ptr<swapchain_ci> create_info = nullptr);
        
        void recreate(std::shared_ptr<swapchain_ci> create_info);

        VkSwapchainKHR& get_vk_swapchain();
        operator VkSwapchainKHR&();

        std::vector<VkImageView>& get_image_views();

        VkFormat& get_format();
        operator VkFormat&();

        VkPresentModeKHR& get_present_mode();
        operator VkPresentModeKHR&();

        void destroy(VkDevice device) const;
        
        ~swapchain();
    };
}


#endif