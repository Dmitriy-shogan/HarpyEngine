#include "..//medium_level_vulkan.hpp"

void harpy_nest::medium_level_vulkan::init_swapchain()
{
    swap_chain_support_details swapchain_support = query_swap_chain_support();

    VkSurfaceFormatKHR surfaceFormat = choose_swapchain_format(swapchain_support.formats);
    VkPresentModeKHR presentMode = choose_swap_present_mode(swapchain_support.presentModes);
    VkExtent2D extent = choose_swap_extent(swapchain_support.capabilities);

    uint32_t image_count {swapchain_support.capabilities.minImageCount + 1};
    
    if (swapchain_support.capabilities.maxImageCount > 0 && image_count > swapchain_support.capabilities.maxImageCount) {
        image_count = swapchain_support.capabilities.maxImageCount;
    }

    
    
    VkSwapchainCreateInfoKHR create_info{};
    create_info.surface = hard_vulk->get_base_window_layout()->get_VK_surface();
    create_info.imageExtent = extent;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surfaceFormat.format;
    create_info.imageColorSpace = surfaceFormat.colorSpace;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto indices = hard_vulk->find_queue_families();
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
    create_info.preTransform = swapchain_support.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    create_info.presentMode = presentMode;
    create_info.clipped = VK_TRUE;

    create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(hard_vulk->get_device(), &create_info, nullptr, nullptr) != VK_SUCCESS)
        throw std::runtime_error("failed to create swap chain!");

    vkGetSwapchainImagesKHR(hard_vulk->get_device(), swapchain, &image_count, nullptr);
    swapchain_images.resize(image_count);
    vkGetSwapchainImagesKHR(hard_vulk->get_device(), swapchain, &image_count, swapchain_images.data());

}
