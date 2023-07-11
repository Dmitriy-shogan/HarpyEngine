#include <renderer_context/renderer_context.h>
#include <windowing/base_window_layout.h>
#include "swapchain.h"
#include <algorithm>

VkDevice& swapchain::get_vk_device() {return r_context->spinal_cord->get_vk_device();}

swapchain::swapchain(renderer_context * r_context): r_context(r_context){

}

swapchain::~swapchain()
        {

            for(auto& i : image_views)
            vkDestroyImageView(r_context->spinal_cord->get_vk_device(), i, nullptr);
			if(chain)
            vkDestroySwapchainKHR(r_context->spinal_cord->get_vk_device(), chain, nullptr);


        }

void swapchain::init()
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r_context->spinal_cord->get_vk_physical_device(), r_context->connected_window_layout->surface, &swapchain_details.capabilities);
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(r_context->spinal_cord->get_vk_physical_device(), r_context->connected_window_layout->surface, &formatCount, nullptr);

	if (formatCount != 0) {
		swapchain_details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(r_context->spinal_cord->get_vk_physical_device(),
			r_context->connected_window_layout->surface,
			&formatCount, swapchain_details.formats.data());
	}

	surface_format = choose_swapchain_format();
	present_mode = choose_swap_present_mode();
	extent = choose_swap_extent();

	uint32_t image_count {
		std::min(
				std::min((uint32_t)swapchain_details.capabilities.minImageCount + 1, (uint32_t)3),
				(uint32_t)swapchain_details.capabilities.maxImageCount
				)};

	if (swapchain_details.capabilities.maxImageCount > 0 && image_count > swapchain_details.capabilities.maxImageCount) {
		image_count = swapchain_details.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = r_context->connected_window_layout->surface;
	create_info.imageExtent = extent;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	//not indices, shit name, needed to be changed
	//std::pair<VkQueue, uint32_t> queue = r_context->spinal_cord->get_queue_supervisor().grab_presentation_queue((VkQueueFlags)0, r_context->connected_window_layout->surface);
	//uint32_t queueFamilyIndices[] = {indices.graphics_families.value(), indices.present_families.value()};

//	if (indices.graphics_families != indices.present_families) {
//		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//		create_info.queueFamilyIndexCount = 2;
//		create_info.pQueueFamilyIndices = queueFamilyIndices;
//	} else {
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	create_info.queueFamilyIndexCount = 0; // Optional
	create_info.pQueueFamilyIndices = nullptr; // Optional
	//}
	create_info.preTransform = swapchain_details.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;

	create_info.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(r_context->spinal_cord->get_vk_device(), &create_info, nullptr, &chain) != VK_SUCCESS)
		throw std::runtime_error("failed to create swap chain!");

	vkGetSwapchainImagesKHR(r_context->spinal_cord->get_vk_device(), chain, &image_count, nullptr);
	images.resize(image_count);
	vkGetSwapchainImagesKHR(r_context->spinal_cord->get_vk_device(), chain, &image_count, images.data());

	init_image_views();

	change_projection(90, static_cast<float>(extent.width)/static_cast<float>(extent.height));
}

void swapchain::reinit()
{

	for(auto& i : image_views)
		vkDestroyImageView(r_context->spinal_cord->get_vk_device(), i, nullptr);
	vkDestroySwapchainKHR(r_context->spinal_cord->get_vk_device(), chain, nullptr);

	vkDeviceWaitIdle(r_context->spinal_cord->get_vk_device());

	int width = 0, height = 0;
	glfwGetFramebufferSize(r_context->connected_window_layout.get()->get_glfw_window(), &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(r_context->connected_window_layout.get()->get_glfw_window(), &width, &height);
		glfwWaitEvents();
	}

	init();
}



VkExtent2D swapchain::choose_swap_extent()
{
	if (swapchain_details.capabilities.currentExtent.width != UINT32_MAX) {
		return swapchain_details.capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(r_context->connected_window_layout.get()->get_glfw_window(), &width, &height);

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
VkSurfaceFormatKHR swapchain::choose_swapchain_format ()
{
	for (const auto& available_format : swapchain_details.formats) {
		if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return available_format;
		}
	}

	return swapchain_details.formats[0];
}
VkPresentModeKHR swapchain::choose_swap_present_mode()
{
	for (const auto& availablePresentMode : swapchain_details.presentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

void swapchain::init_image_views()
{
	image_views.resize(images.size());
	fbs.resize(images.size());
	image_sems.resize(images.size());
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

	VkFramebufferCreateInfo framebuffer_info{};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = r_context->render_pass;
	framebuffer_info.attachmentCount = 1;
	framebuffer_info.width = r_context->swapchain.get_vk_extent().width;
	framebuffer_info.height = r_context->swapchain.get_vk_extent().height;
	framebuffer_info.layers = 1;

	VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for(int f = 0; f < images.size(); f++)
	{
		create_info.image = images[f];
		if (vkCreateImageView(r_context->spinal_cord->device, &create_info, nullptr, &image_views[f]) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
		}
		framebuffer_info.pAttachments = &image_views[f];

		if (vkCreateFramebuffer(r_context->spinal_cord->device, &framebuffer_info, nullptr, &fbs[f]) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init framebuffer!");
		}

		if (vkCreateSemaphore(r_context->spinal_cord->device, &semaphoreInfo, nullptr, &image_sems[f]) != VK_SUCCESS)
			        	throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphores!");

	}




}




