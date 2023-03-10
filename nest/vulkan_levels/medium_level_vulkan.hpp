#pragma once
#ifndef HARPY_MEDIUM_VULKAN
#define HARPY_MEDIUM_VULKAN
#include "hard_level_vulkan.hpp"

namespace harpy_nest{
    
class medium_level_vulkan : public hard_level_vulkan
{
protected:
    
    struct swap_chain_support_details {
        std::vector<VkSurfaceFormatKHR> formats;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    VkSwapchainKHR swapchain{nullptr};
    std::vector<VkImage> swapchain_images{};
    VkSurfaceFormatKHR surface_format {};
    VkPresentModeKHR present_mode {};
    VkExtent2D extent {};

    std::vector<VkImageView> swapchain_image_views;
    
    swap_chain_support_details query_swap_chain_support() const;

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

    void init_image_views()
    {
        swapchain_image_views.resize(swapchain_images.size());

        VkImageViewCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = surface_format.format;

        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;


        for(int f = 0; auto& i : swapchain_images)
        {
            create_info.image = i;
            if (vkCreateImageView(device, &create_info, nullptr, &swapchain_image_views[f++]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }
    void init_swapchain();

    medium_level_vulkan() : hard_level_vulkan(){}
    
public:
    static VkSurfaceFormatKHR choose_swapchain_format(const std::vector<VkSurfaceFormatKHR>& available_formats);

    static VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    void init_default_softer()
    {
        init_default_hard();
        init_swapchain();
        init_image_views();
    };

    ~medium_level_vulkan() override
    {
        for (auto image_view : swapchain_image_views) {
            vkDestroyImageView(device, image_view, nullptr);
        }
        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
};
    
}

#endif