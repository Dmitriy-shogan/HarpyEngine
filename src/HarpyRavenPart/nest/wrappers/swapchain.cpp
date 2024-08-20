﻿#include "nest/wrappers/swapchain.h"

#include <algorithm>
#include <utility>
#include "nest/resources/common_vulkan_resource.h"
#include "nest/inititalizations.h"
#include "nest/resources/surface_capabilities.h"

using resource = harpy::nest::resources::common_vulkan_resource;
using harpy_error = harpy::utilities::harpy_little_error;
using error_enum = harpy::utilities::error_severity;

//TODO: populate with debug info
void harpy::nest::wrappers::swapchain::init(
	swapchain_ci* create_info,
	VkDevice* device)
{
	this->device = device;
	bool checker{false};
	if (create_info == nullptr)
	{
		checker = true;
		create_info = new swapchain_ci{};
	}
	for(int counter = 0; auto& i : create_info->caps.formats)
	{
		for(auto& f : create_info->ideal_formats)
		{
			if (i.format == f)
			{
				format = i.format;
				color_space = i.colorSpace;
                           
				//YEAH THIS IS GOTO IT GOES ABOUT 10 ROWS BELOW
				goto loop_exit;
			}
		}
		++counter;
	}
                           	
	if (!format) throw harpy_error("Can't find correct format for swapchain.");
                           
	//YEAH THIS IS WHERE LOOP EXIT GOES YEAH
	loop_exit:
                           	
	//TODO: make v-sync and make out present modes
	for(auto& i : create_info->caps.present_modes)
	{
		//Here must be v_sync check for correctly finding present mode, but later, when v_sync integration will be
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
	ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                           
	//Just for now
	ci.queueFamilyIndexCount = 1;
                           	
	ci.pQueueFamilyIndices = &create_info->family_index;
	ci.imageSharingMode = (ci.queueFamilyIndexCount > 1) ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE;
                           	
	ci.preTransform = create_info->caps.capabilities.currentTransform;
                           	
	if (create_info->caps.composite_alpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
        if (create_info->caps.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
            ci.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

        else if (create_info->caps.capabilities.supportedCompositeAlpha &
                 VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR)
            ci.compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;

        else if (create_info->caps.capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR)
            ci.compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
        else
            ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    }
                           	
	ci.presentMode = present_mode;
	ci.oldSwapchain = create_info->old_swapchain;
                           
	//Just for now
	ci.clipped = true;
                           
	HARPY_VK_CHECK(vkCreateSwapchainKHR(resource::get_resource(), &ci, nullptr, &chain));
	init_image_views();
    depth_image.init(extent);
	pass.init(format);
	init_framebuffers();
	if(checker)
		delete create_info;
}

void harpy::nest::wrappers::swapchain::destroy(bool do_delete_vk_swapchain) const
{
	if(chain)
	{

		for(auto& f : framebuffers)
			vkDestroyFramebuffer(*device, f, nullptr);
		
		for(auto& i : views)
			vkDestroyImageView(*device, i, nullptr);

		if(do_delete_vk_swapchain)
		vkDestroySwapchainKHR(*device, chain, nullptr);
	}
}

//OH JEEZ
//Todo: deleting old swapchain must be AFTER creation of the new one
void harpy::nest::wrappers::swapchain::recreate(swapchain_ci* create_info, VkDevice* device)
{
    destroy();
	create_info->old_swapchain = chain;
	init(create_info, device);
}


void harpy::nest::wrappers::swapchain::init_image_views()
{
	uint32_t images_count{};
	vkGetSwapchainImagesKHR(resource::get_resource(), chain, &images_count, nullptr);
	images.resize(images_count);
	vkGetSwapchainImagesKHR(resource::get_resource(), chain, &images_count, images.data());

	views.resize(images_count);
	for(size_t counter = 0; auto& i : images)
	{
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

		HARPY_VK_CHECK(vkCreateImageView(resource::get_resource(), &ci, nullptr, &views[counter++]));
	}
	
}

void harpy::nest::wrappers::swapchain::init_framebuffers()
{
	framebuffers.resize(views.size());
    std::vector<VkImageView> attachments{2};
    attachments[1] = depth_image.get_vk_image_view();
	
	for(int counter = 0; auto& i : framebuffers)
	{
		VkFramebufferCreateInfo frame_ci{};
        attachments[0] = views[counter++];
		frame_ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frame_ci.renderPass = pass.get_vk_render_pass();
		frame_ci.attachmentCount = attachments.size();
		frame_ci.pAttachments = attachments.data();
		frame_ci.width = extent.width;
		frame_ci.height = extent.height;
		frame_ci.layers = 1;

		vkCreateFramebuffer(resource::get_resource(), &frame_ci, nullptr, &i);
	}
}

harpy::nest::wrappers::swapchain::swapchain(swapchain_ci* create_info, VkDevice* device)
{
	init(create_info, device);
    depth_image = resources::depth_image(extent);
}


VkSwapchainKHR& harpy::nest::wrappers::swapchain::get_vk_swapchain()
{return chain;}

harpy::nest::wrappers::swapchain::operator VkSwapchainKHR_T*&()
{return chain;}

std::vector<VkImageView>& harpy::nest::wrappers::swapchain::get_image_views()
{return views;}

VkFramebuffer& harpy::nest::wrappers::swapchain::get_framebuffer(size_t index)
{return framebuffers[index];
}

VkFormat& harpy::nest::wrappers::swapchain::get_format()
{return format;}

harpy::nest::wrappers::swapchain::operator VkFormat&()
{return format;}

VkPresentModeKHR& harpy::nest::wrappers::swapchain::get_present_mode()
{return present_mode;}

harpy::nest::wrappers::swapchain::operator VkPresentModeKHR&()
{return present_mode;}

VkExtent2D& harpy::nest::wrappers::swapchain::get_extent()
{return extent;
}

harpy::nest::wrappers::swapchain::operator VkExtent2D&()
{return extent;
}

uint32_t harpy::nest::wrappers::swapchain::acquire_vk_image_index(threading::semaphore* semaphore_to_signal,
                                                                  threading::fence* fence_to_signal,
                                                                  size_t timeout)
{
	uint32_t vk_image_index{};
    HARPY_VK_CHECK(vkAcquireNextImageKHR(*device, chain,
                          timeout,
                                         semaphore_to_signal ? semaphore_to_signal->get_vk_semaphore() : nullptr,
                          fence_to_signal ? fence_to_signal->get_vk_fence() : nullptr,
                          &vk_image_index));

    return vk_image_index;
}

harpy::nest::wrappers::render_pass& harpy::nest::wrappers::swapchain::get_render_pass()
{return pass;
}

harpy::nest::wrappers::swapchain::~swapchain()
{
	if(pass.get_vk_render_pass())
	{
		for(auto& f : framebuffers)
			vkDestroyFramebuffer(*device, f, nullptr);
		
		for(auto& i : views)
			vkDestroyImageView(*device, i, nullptr);
	}
	if (chain)
		vkDestroySwapchainKHR(*device, chain, nullptr);
}

VkFormat harpy::nest::wrappers::swapchain::get_depth_format() {
    for (VkFormat format :  {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(resource::get_resource().get_phys_device(), format, &props);

        if ((props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}