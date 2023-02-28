#pragma once
#ifndef HARPY_MEDIUM_VULKAN
#define HARPY_MEDIUM_VULKAN
#include "hard_level_vulkan.hpp"

namespace harpy_nest{
    
class medium_level_vulkan
{
    struct swap_chain_support_details {
        std::vector<VkSurfaceFormatKHR> formats;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    hard_level_vulkan* hard_vulk;
    

    VkSwapchainKHR swapchain{nullptr};
    std::vector<VkImage> swapchain_images{};
    
    swap_chain_support_details query_swap_chain_support() const
    {
        swap_chain_support_details details{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(hard_vulk->get_ph_device(), hard_vulk->get_base_window_layout()->get_VK_surface(), &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(hard_vulk->get_ph_device(), hard_vulk->get_base_window_layout()->get_VK_surface(), &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(hard_vulk->get_ph_device(), hard_vulk->get_base_window_layout()->get_VK_surface(), &formatCount, details.formats.data());
        }

        return details;
    }

    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(&hard_vulk->get_base_window_layout()->get_glfw_window(), &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    
    
public:
    
    medium_level_vulkan(hard_level_vulkan& vulk) : hard_vulk(&vulk) {}
    
    static VkSurfaceFormatKHR choose_swapchain_format(const std::vector<VkSurfaceFormatKHR>& available_formats)
    {
        for (const auto& available_format : available_formats) {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return available_format;
            }
        }

        return available_formats[0];
    }
    
    static VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    void init_swapchain();


    void init_all_default();

    ~medium_level_vulkan()
    {
        vkDestroySwapchainKHR(hard_vulk->get_device(), swapchain, nullptr);
        hard_vulk->~hard_level_vulkan();
    }
};
    
}

#endif