/*
 * render_shared_resources.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */



#include "render_shared_resources.h"
#include <renderer_context/renderer_context.h>
#include <swapchain/swapchain.h>
#include <utilities/utilities.h>




void render_shared_resources::init_images(){


	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

	imageCreateInfo.extent = { r_context->swapchain.extent.width, r_context->swapchain.extent.height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	VkMemoryRequirements memoryRequirements;
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkImageViewCreateInfo imageview_create_info{};
	imageview_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	imageview_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageview_create_info.format = r_context->swapchain.surface_format.format;

	imageview_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageview_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageview_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageview_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	imageview_create_info.subresourceRange.baseMipLevel = 0;
	imageview_create_info.subresourceRange.levelCount = 1;
	imageview_create_info.subresourceRange.baseArrayLayer = 0;
	imageview_create_info.subresourceRange.layerCount = 1;

	//	========================
	//	  COLOR_BUFFER
	//	========================
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	vkGetImageMemoryRequirements(r_context->spinal_cord->device, color_image, &memoryRequirements);
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = harpy::utilities::find_memory_types(r_context->spinal_cord->ph_device, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	imageview_create_info.image = color_image;
	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

	if (vkCreateImage(r_context->spinal_cord->device, &imageCreateInfo, nullptr, &color_image) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image for RSR!");

	if (vkAllocateMemory(r_context->spinal_cord->device, &allocateInfo, nullptr, &color_image_memory) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to allocate image for RSR!");

	if (vkBindImageMemory(r_context->spinal_cord->device, color_image, color_image_memory, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to bind image for RSR!");

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &color_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");

	//	========================
	//	  DEPTH_AND_STENSIL_BUFFER
	//	========================

	imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	vkGetImageMemoryRequirements(r_context->spinal_cord->device, depth_and_stencil_image, &memoryRequirements);
	allocateInfo.allocationSize = memoryRequirements.size;
	allocateInfo.memoryTypeIndex = harpy::utilities::find_memory_types(r_context->spinal_cord->ph_device, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	imageview_create_info.image = depth_and_stencil_image;
	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

	if (vkCreateImage(r_context->spinal_cord->device, &imageCreateInfo, nullptr, &depth_and_stencil_image) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image for RSR!");

	if (vkAllocateMemory(r_context->spinal_cord->device, &allocateInfo, nullptr, &depth_and_stencil_image_memory) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to allocate image for RSR!");

	if (vkBindImageMemory(r_context->spinal_cord->device, depth_and_stencil_image, depth_and_stencil_image_memory, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to bind image for RSR!");

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &depth_and_stencil_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");




	VkImageView attachments[] = {color_image_view, depth_and_stencil_image_view};
	VkFramebufferCreateInfo framebuffer_info{};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = r_context->render_pass;
	framebuffer_info.attachmentCount = 2;
	framebuffer_info.pAttachments = attachments;
	framebuffer_info.width = r_context->swapchain.get_vk_extent().width;
	framebuffer_info.height = r_context->swapchain.get_vk_extent().height;
	framebuffer_info.layers = 1;


	if (vkCreateFramebuffer(r_context->spinal_cord->device, &framebuffer_info, nullptr, &fb) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init framebuffer!");

}
void render_shared_resources::init_sem(){
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(r_context->spinal_cord->device, &semaphoreInfo, nullptr, &sem) != VK_SUCCESS)
							throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphores!");

}

void render_shared_resources::init_command_buffer(){
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = command_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(r_context->spinal_cord->device, &allocInfo, &cmd) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}


	VkCommandBufferBeginInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	create_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	if(vkBeginCommandBuffer(cmd, &create_info) != VK_SUCCESS)
					throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Can't start writing command buffer!");

}

void render_shared_resources::wait(){
	VkSemaphore semaphores[] = {sem,sem2};
	VkSemaphoreWaitInfo waitInfo{};
	waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	waitInfo.semaphoreCount = 2;
	waitInfo.pSemaphores = semaphores;
	waitInfo.pValues = nullptr;

	if (vkWaitSemaphores(r_context->spinal_cord->device,&waitInfo,UINT64_MAX) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to wait RSR semaphores!");
}

void render_shared_resources::reset(){
	if (vkResetCommandBuffer(cmd, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to reset cmd RSR!");

	queue.empty();
}

render_shared_resources::~render_shared_resources(){
	if(cmd)
	vkEndCommandBuffer(cmd);
	if(cmd)
	vkFreeCommandBuffers(r_context->spinal_cord->device, command_pool, 1, &(this->cmd));
	if(fb)
	vkDestroyFramebuffer(r_context->spinal_cord->device,this->fb,nullptr);

	if(color_image_view)
	vkDestroyImageView(r_context->spinal_cord->device,this->color_image_view,nullptr);
	if(color_image)
	vkDestroyImage(r_context->spinal_cord->device,this->color_image,nullptr);
	if(color_image_memory)
	vkFreeMemory(r_context->spinal_cord->device, color_image_memory, nullptr);

	if(depth_and_stencil_image_view)
	vkDestroyImageView(r_context->spinal_cord->device,this->depth_and_stencil_image_view,nullptr);
	if(depth_and_stencil_image)
	vkDestroyImage(r_context->spinal_cord->device,this->depth_and_stencil_image,nullptr);
	if(depth_and_stencil_image_memory)
	vkFreeMemory(r_context->spinal_cord->device, depth_and_stencil_image_memory, nullptr);


	if(sem)
	vkDestroySemaphore(r_context->spinal_cord->device,sem,nullptr);
}


