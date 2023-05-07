#pragma once
#ifndef HARPY_SWAPCHAIN
#define HARPY_SWAPCHAIN
#include <utilities/harpy_little_error.h>

#include <buffers/framebuffer.h>

namespace harpy::nest
{
    struct swapchain_support_details  {
        std::vector<VkSurfaceFormatKHR> formats;
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    class swapchain : public interfaces::IStrong_component
    {
        hard_level_vulkan& vulkan_backend;
        render_pass& rend;
        
        VkSwapchainKHR chain{nullptr};
        std::vector<buffers::framebuffer> framebuffs;
        std::vector<VkImageView> image_views;
        std::vector<VkImage> images;

        VkSurfaceFormatKHR surface_format {};
        VkPresentModeKHR present_mode {};
        VkExtent2D extent {};
        swapchain_support_details swapchain_details{};
        
        VkExtent2D choose_swap_extent()
        {
            if (swapchain_details.capabilities.currentExtent.width != UINT32_MAX) {
                return swapchain_details.capabilities.currentExtent;
            } else {
                int width, height;
                glfwGetFramebufferSize(vulkan_backend.get_window_layout().get_glfw_window(), &width, &height);

                VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                };

                actualExtent.width = std::max(swapchain_details.capabilities.minImageExtent.width,
                    std::min(swapchain_details.capabilities.maxImageExtent.width, actualExtent.width));
                actualExtent.height = std::max(swapchain_details.capabilities.minImageExtent.height,
                    std::min(swapchain_details.capabilities.maxImageExtent.height, actualExtent.height));

                return actualExtent;
            }
        }
        VkSurfaceFormatKHR choose_swapchain_format ()
        {
            for (const auto& available_format : swapchain_details.formats) {
                if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return available_format;
                }
            }

            return swapchain_details.formats[0];
        }
        VkPresentModeKHR medium_level_vulkan::choose_swap_present_mode()
        {
            for (const auto& availablePresentMode : swapchain_details.presentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
            }

            return VK_PRESENT_MODE_FIFO_KHR;
        }

        void init_image_views()
        {
            image_views.resize(images.size());

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


            for(int f = 0; auto& i : images)
            {
                create_info.image = i;
                if (vkCreateImageView(vulkan_backend.get_vk_device(), &create_info, nullptr, &image_views[f++]) != VK_SUCCESS) {
                    throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
                }
            }   
        }
        
    public:
        
        swapchain(hard_level_vulkan& vulkan_backend, render_pass& rend): vulkan_backend(vulkan_backend), rend(rend){}
        
        VkDevice& get_vk_device() override {return vulkan_backend.get_vk_device();}

        void init()
        {
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkan_backend.get_vk_physical_device(), vulkan_backend.get_vk_surface(), &swapchain_details.capabilities);
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_backend.get_vk_physical_device(), vulkan_backend.get_vk_surface(), &formatCount, nullptr);

            if (formatCount != 0) {
                swapchain_details.formats.resize(formatCount);
                vkGetPhysicalDeviceSurfaceFormatsKHR(vulkan_backend.get_vk_physical_device(),
                    vulkan_backend.get_vk_surface(),
                    &formatCount, swapchain_details.formats.data());
            }
            
            surface_format = choose_swapchain_format();
            present_mode = choose_swap_present_mode();
            extent = choose_swap_extent();

            uint32_t image_count {swapchain_details.capabilities.minImageCount + 1};
    
            if (swapchain_details.capabilities.maxImageCount > 0 && image_count > swapchain_details.capabilities.maxImageCount) {
                image_count = swapchain_details.capabilities.maxImageCount;
            }
    
            VkSwapchainCreateInfoKHR create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            create_info.surface = vulkan_backend.get_vk_surface();
            create_info.imageExtent = extent;
            create_info.minImageCount = image_count;
            create_info.imageFormat = surface_format.format;
            create_info.imageColorSpace = surface_format.colorSpace;
            create_info.imageArrayLayers = 1;
            create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

            //not indices, shit name, needed to be changed
            auto indices = hard_level_vulkan::find_queue_families(
                vulkan_backend.get_vk_physical_device(),
                vulkan_backend.get_vk_surface()
                );
            uint32_t queueFamilyIndices[] = {indices.graphics_families.value(), indices.present_families.value()};

            if (indices.graphics_families != indices.present_families) {
                create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
                create_info.queueFamilyIndexCount = 2;
                create_info.pQueueFamilyIndices = queueFamilyIndices;
            } else {
                create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                create_info.queueFamilyIndexCount = 0; // Optional
                create_info.pQueueFamilyIndices = nullptr; // Optional
            }
            create_info.preTransform = swapchain_details.capabilities.currentTransform;
            create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

            create_info.presentMode = present_mode;
            create_info.clipped = VK_TRUE;

            create_info.oldSwapchain = VK_NULL_HANDLE;

            if (vkCreateSwapchainKHR(vulkan_backend.get_vk_device(), &create_info, nullptr, &chain) != VK_SUCCESS)
                throw std::runtime_error("failed to create swap chain!");

            vkGetSwapchainImagesKHR(vulkan_backend.get_vk_device(), chain, &image_count, nullptr);
            images.resize(image_count);
            vkGetSwapchainImagesKHR(vulkan_backend.get_vk_device(), chain, &image_count, images.data());
            
            init_image_views();
            
            framebuffs.resize(image_views.size(), {rend});
            for(int f = 0;auto& i : framebuffs)
            {
                i.init(extent, image_views[f++]);
            }
        }

        void reinit()
        {
            for(auto& i : framebuffs)
                vkDestroyFramebuffer(vulkan_backend.get_vk_device(), i, nullptr);
            for(auto& i : image_views)
                vkDestroyImageView(vulkan_backend.get_vk_device(), i, nullptr);
            vkDestroySwapchainKHR(vulkan_backend.get_vk_device(), chain, nullptr);
            
            vkDeviceWaitIdle(vulkan_backend.get_vk_device());

            int width = 0, height = 0;
            glfwGetFramebufferSize(vulkan_backend.get_window_layout().get_glfw_window(), &width, &height);
            while (width == 0 || height == 0) {
                glfwGetFramebufferSize(vulkan_backend.get_window_layout().get_glfw_window(), &width, &height);
                glfwWaitEvents();
            }

            init();
        }
        



        std::vector<buffers::framebuffer>& get_framebuffers(){return framebuffs;}
        std::vector<VkImageView>& get_image_views(){return image_views;}
        std::vector<VkImage>& get_images(){return images;}

        VkSurfaceFormatKHR& get_vk_surface_format() {return surface_format;}
        VkPresentModeKHR& get_vk_present_mode() {return present_mode;}
        VkExtent2D& get_vk_extent() {return extent;}
        swapchain_support_details& get_vk_swapchain_details(){return swapchain_details;}
        VkSwapchainKHR& get_vk_swapchain(){return chain;}
        operator VkSwapchainKHR&(){return chain;}

        ~swapchain() override
        {
            for(auto& i : image_views)
            vkDestroyImageView(vulkan_backend.get_vk_device(), i, nullptr);
            
            vkDestroySwapchainKHR(vulkan_backend.get_vk_device(), chain, nullptr);
            
        }
        
    
    };
}
#endif //HARPY_SWAPCHAIN