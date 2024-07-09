#pragma once
#ifndef HARPY_NEST_WRAPPERS_SWAPCHAIN
#define HARPY_NEST_WRAPPERS_SWAPCHAIN
#include <nest/resources/common_vulkan_resource.h>
#include <nest/resources/surface_capabilities.h>
#include <nest/vulkan_threading/fence.h>
#include <nest/vulkan_threading/semaphor.h>

namespace harpy::nest::wrappers
{
    struct swapchain_ci{
        windowing::window& win {resources::common_vulkan_resource::get_resource().get_main_window()};
        VkSurfaceKHR surface{resources::common_vulkan_resource::get_resource()};
        uint32_t family_index {resources::common_vulkan_resource::get_resource().get_main_family_queue().get_family_index()};
        std::vector<VkFormat> ideal_formats {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_R8G8B8_SINT};
        VkDevice device = resources::common_vulkan_resource::get_resource().get_main_device();
        resources::surface_capabilities caps {resources::std_surface_capabilities};
        VkSwapchainKHR old_swapchain{nullptr};
        VkRenderPassCreateInfo2 ci = {};
    };
    
    class swapchain
    {
        VkSwapchainKHR chain{};
        std::vector<VkImageView> views{};
        std::vector<VkImage> images{};
        std::vector<VkFramebuffer> framebuffers{};
        VkRenderPass render_pass;
        
        VkFormat format{};
        VkColorSpaceKHR color_space{};
        VkPresentModeKHR present_mode{};
        VkExtent2D extent{};

        void init_render_pass(VkRenderPassCreateInfo2 ci = {});
        void init_image_views();
        void init_framebuffers();

        void init(swapchain_ci* create_info = nullptr,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        VkDevice* device{nullptr};
        
    public:

        swapchain(swapchain_ci* create_info = nullptr,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        void recreate(swapchain_ci* create_info = nullptr,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        VkSwapchainKHR& get_vk_swapchain();
        operator VkSwapchainKHR&();

        std::vector<VkImageView>& get_image_views();

        VkFramebuffer& get_framebuffer(size_t index);

        VkFormat& get_format();
        operator VkFormat&();

        VkPresentModeKHR& get_present_mode();
        operator VkPresentModeKHR&();

        VkExtent2D& get_extent();
        operator VkExtent2D&();

        uint32_t acquire_vk_image_index(threading::semaphore semaphore, threading::fence* fence, size_t timeout = LLONG_MAX); 

        VkRenderPass& get_render_pass();

        void destroy(bool do_destroy_vk_swapchain = false) const;
        
        ~swapchain();
    };
}


#endif