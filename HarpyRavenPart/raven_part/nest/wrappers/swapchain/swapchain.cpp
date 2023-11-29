#include "swapchain.h"

#include <algorithm>
#include <utility>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/initializations/inititalizations.h>
#include <nest/resources/surface_capabilities.h>
#include <utilities/logger/harpy_little_error.h>
#include <nest/wrappers/render_pass/render_pass.h>

using resource = harpy::nest::resources::common_vulkan_resource;
using harpy_error = harpy::utilities::error_handling::harpy_little_error;
using error_enum = harpy::utilities::error_handling::error_severity;

struct harpy::nest::wrappers::swapchain_ci{
	windowing::window& win {resource::get_resource().get_main_window()};
	VkSurfaceKHR surface{resource::get_resource()};
	uint32_t family_index {resource::get_resource().get_main_family_queue().get_family_index()};
	std::vector<VkFormat> ideal_formats {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB};
	VkDevice device = resources::common_vulkan_resource::get_resource().get_main_device();
	resources::surface_capabilities caps {resources::std_surface_capabilities};
	VkSwapchainKHR old_swapcain{nullptr};
	render_pass pass{};
};

//TODO: populate with debug info
void harpy::nest::wrappers::swapchain::init(
	std::shared_ptr<swapchain_ci> create_info = nullptr)
{
	if (create_info == nullptr)
	{
		create_info = std::make_shared<swapchain_ci>();
	}
	for(int counter = 0; auto& i : create_info->caps.formats)
	{
		for(auto& f : create_info->ideal_formats)
		{
			if (i == f)
			{
				format = i;
				color_space = create_info->caps.color_spaces[counter];
			}
		}
		++counter;
	}
	
	if (!format) throw harpy_error("Can't find correct format for swapchain.");

	//TODO: make v-sync and make out present modes
	for(auto& i : create_info->caps.present_modes)
	{
		//Here must be v_sync check for correctly finding present mode, but later, when v_synk integration will be
		if(i == VK_PRESENT_MODE_MAILBOX_KHR || i == VK_PRESENT_MODE_FIFO_KHR)
		{
			present_mode = i;
			break;
		}
	}
	
	if(!present_mode) throw harpy_error("Can't find correct present mode for swapchain.");
    
	int width, height;
	glfwGetFramebufferSize(create_info->win, &width, &height);
    
	extent.width = std::clamp(
		static_cast<uint32_t>(width),
		create_info->caps.capabilities.minImageExtent.width,
		create_info->caps.capabilities.maxImageExtent.width);
    
	extent.height = std::clamp(
		static_cast<uint32_t>(height),
		create_info->caps.capabilities.minImageExtent.height,
		create_info->caps.capabilities.maxImageExtent.height);
    
	VkSwapchainCreateInfoKHR ci = {};
	ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	ci.surface = resource::get_resource();
	ci.minImageCount = MAX_FRAMES_IN_FLIGHT;
	ci.imageFormat = format;
	ci.imageColorSpace = color_space;
	ci.imageExtent = extent;
	ci.imageArrayLayers = 1;
	
	//Not sure about storage usage bit, needs research
	ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

	//Just for now
	ci.queueFamilyIndexCount = 1;
	
	ci.pQueueFamilyIndices = &create_info->family_index;
	ci.imageSharingMode = (ci.queueFamilyIndexCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
	
	ci.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	ci.compositeAlpha = create_info->caps.composite_alpha;
	ci.presentMode = present_mode;
	ci.oldSwapchain = create_info->old_swapcain;

	//Just for now
	ci.clipped = true;
	
	ci.oldSwapchain = VK_NULL_HANDLE;

	HARPY_VK_CHECK(vkCreateSwapchainKHR(resource::get_resource(), &ci, nullptr, &chain));

	init_framebuffers(create_info->pass);
}

void harpy::nest::wrappers::swapchain::destroy(VkDevice device = resource::get_resource().get_main_device()) const
{
	if(chain)
	{
		vkDestroySwapchainKHR(device, chain, nullptr);

		for(auto& f : framebuffers)
		{
			vkDestroyFramebuffer(device, f, nullptr);
		}
		for(auto& i : views)
			vkDestroyImageView(device, i, nullptr);
	}
}


void harpy::nest::wrappers::swapchain::recreate(std::shared_ptr<swapchain_ci> create_info = nullptr)
{
	destroy();
	init(std::move(create_info));
}


void harpy::nest::wrappers::swapchain::init_image_views()
{
	uint32_t images_count{};
	vkGetSwapchainImagesKHR(resource::get_resource(), chain, &images_count, nullptr);
	images.resize(images_count);
	vkGetSwapchainImagesKHR(resource::get_resource(), chain, &images_count, images.data());

	for(auto& i : images)
	{
		views.emplace_back();
		VkImageViewCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ci.image = i;
		ci.format = format;
		
		//Just for now, needs research
		ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
		
		ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//Just for now, needs research
		ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ci.subresourceRange.baseMipLevel = 0;
		ci.subresourceRange.levelCount = 1;
		ci.subresourceRange.baseArrayLayer = 0;
		ci.subresourceRange.layerCount = 1;

		HARPY_VK_CHECK(vkCreateImageView(resource::get_resource(), &ci, nullptr, &views.back()));
	}
	
}

void harpy::nest::wrappers::swapchain::init_framebuffers(render_pass pass)
{
	framebuffers.resize(views.size());
	
	for(int counter = 0; auto& i : framebuffers)
	{
		VkFramebufferCreateInfo frame_ci{};
	
		frame_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_ci.renderPass = pass;
		frame_ci.attachmentCount = 1;
		frame_ci.pAttachments = &views[counter];
		frame_ci.width = extent.width;
		frame_ci.height = extent.height;
		frame_ci.layers = 1;

		vkCreateFramebuffer(resource::get_resource(), &frame_ci, nullptr, &i);
	}
}


VkSwapchainKHR& harpy::nest::wrappers::swapchain::get_vk_swapchain()
{return chain;}

harpy::nest::wrappers::swapchain::operator VkSwapchainKHR_T*&()
{return chain;}

std::vector<VkImageView>& harpy::nest::wrappers::swapchain::get_image_views()
{return views;}

VkFormat& harpy::nest::wrappers::swapchain::get_format()
{return format;}

harpy::nest::wrappers::swapchain::operator VkFormat&()
{return format;}

VkPresentModeKHR& harpy::nest::wrappers::swapchain::get_present_mode()
{return present_mode;}

harpy::nest::wrappers::swapchain::operator VkPresentModeKHR&()
{return present_mode;}


harpy::nest::wrappers::swapchain::~swapchain()
{
	if(chain)
	{
		vkDestroySwapchainKHR(resource::get_resource(), chain, nullptr);

		for(auto& f : framebuffers)
		{
			vkDestroyFramebuffer(resource::get_resource(), f, nullptr);
		}
		for(auto& i : views)
			vkDestroyImageView(resource::get_resource(), i, nullptr);
	}
}
