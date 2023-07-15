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
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.extent = { r_context->swapchain.extent.width, r_context->swapchain.extent.height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	//VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


	VkMemoryRequirements memoryRequirements;
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	VkImageViewCreateInfo imageview_create_info{};
	imageview_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;

	imageview_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	//imageview_create_info.format = //r_context->swapchain.surface_format.format;

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
	imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	//imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageview_create_info.format = VK_FORMAT_R8G8B8A8_UNORM;

	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	std::cout<<"init start"<<std::endl;
	if (vkCreateImage(r_context->spinal_cord->device, &imageCreateInfo, nullptr, &color_image) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image for RSR!");
	std::cout<<"init vkCreateImage"<<std::endl;
	vkGetImageMemoryRequirements(r_context->spinal_cord->device, color_image, &memoryRequirements);
	allocateInfo.memoryTypeIndex = harpy::utilities::find_memory_types(r_context->spinal_cord->ph_device, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	allocateInfo.allocationSize = memoryRequirements.size;

	if (vkAllocateMemory(r_context->spinal_cord->device, &allocateInfo, nullptr, &color_image_memory) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to allocate image for RSR!");
	std::cout<<"init vkAllocateMemory"<<std::endl;
	if (vkBindImageMemory(r_context->spinal_cord->device, color_image, color_image_memory, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to bind image for RSR!");
	std::cout<<"init vkBindImageMemory"<<std::endl;
	imageview_create_info.image = color_image;

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &color_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
	std::cout<<"init vkCreateImageView"<<std::endl;
	//	========================
	//	  DEPTH_AND_STENSIL_BUFFER
	//	========================

	imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	//imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	imageview_create_info.format = VK_FORMAT_D24_UNORM_S8_UINT;

	std::cout<<"init start2"<<std::endl;
	if (vkCreateImage(r_context->spinal_cord->device, &imageCreateInfo, nullptr, &depth_and_stencil_image) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image for RSR!");
	std::cout<<"init vkCreateImage"<<std::endl;
	vkGetImageMemoryRequirements(r_context->spinal_cord->device, depth_and_stencil_image, &memoryRequirements);
	allocateInfo.memoryTypeIndex = harpy::utilities::find_memory_types(r_context->spinal_cord->ph_device, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	allocateInfo.allocationSize = memoryRequirements.size;
	if (vkAllocateMemory(r_context->spinal_cord->device, &allocateInfo, nullptr, &depth_and_stencil_image_memory) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to allocate image for RSR!");
	std::cout<<"init vkAllocateMemory"<<std::endl;
	if (vkBindImageMemory(r_context->spinal_cord->device, depth_and_stencil_image, depth_and_stencil_image_memory, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to bind image for RSR!");
	std::cout<<"init vkBindImageMemory"<<std::endl;
	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageview_create_info.image = depth_and_stencil_image;

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &depth_and_stencil_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
	std::cout<<"init vkCreateImageView"<<std::endl;


	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	imageview_create_info.image = depth_and_stencil_image;

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &depth_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
	std::cout<<"init vkCreateImageView"<<std::endl;


	imageview_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
	imageview_create_info.image = depth_and_stencil_image;

	if (vkCreateImageView(r_context->spinal_cord->device, &imageview_create_info, nullptr, &stencil_image_view) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image views!");
	std::cout<<"init vkCreateImageView"<<std::endl;



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

//	VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo = {};
//	semaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO_KHR;
//	semaphoreTypeCreateInfo.pNext = nullptr;
//	semaphoreTypeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
//	semaphoreTypeCreateInfo.initialValue = 0;

	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//	semaphoreInfo.pNext = &semaphoreTypeCreateInfo;
	if (vkCreateSemaphore(r_context->spinal_cord->device, &semaphoreInfo, nullptr, &sem) != VK_SUCCESS)
							throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphore!");

	if (vkCreateSemaphore(r_context->spinal_cord->device, &semaphoreInfo, nullptr, &sem2) != VK_SUCCESS)
								throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create semaphore!");

//	VkSemaphoreSignalInfo signal{};
//	signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
//	signal.value = 1;
//	signal.semaphore = sem;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");
//	signal.semaphore = sem2;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");

//	curr_sem_val = 0;
//	curr_sem2_val = 0;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	if (vkCreateFence(r_context->spinal_cord->device, &fenceInfo, nullptr, &fence1) != VK_SUCCESS)
							throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create fence!");

	if (vkCreateFence(r_context->spinal_cord->device, &fenceInfo, nullptr, &fence2) != VK_SUCCESS)
								throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create fence!");

	VkFence fences[] = {fence1,fence2};
	if (vkResetFences(r_context->spinal_cord->device, 2, fences) != VK_SUCCESS)
								throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to reset fence!");
}



void render_shared_resources::wait(){
	VkFence fences[] = {fence1,fence2};
	if(wait_needed){
		if (vkWaitForFences(r_context->spinal_cord->device, 2, fences, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to wait RSR fences!");
	}
	wait_needed = true;
//	VkSemaphore semaphores[] = {sem,sem2};
//	uint64_t values[] = {++curr_sem_val, ++curr_sem2_val};
//	VkSemaphoreWaitInfo waitInfo{};
//	waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
//	waitInfo.semaphoreCount = 2;
//	waitInfo.pSemaphores = semaphores;
//	waitInfo.pValues = values;
//
//	if (vkWaitSemaphores(r_context->spinal_cord->device,&waitInfo,UINT64_MAX) != VK_SUCCESS)
//		throw utilities::harpy_little_error("failed to wait RSR semaphores!");

//	VkSemaphoreSignalInfo signal{};
//	signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
//	signal.value = 0;
//	signal.semaphore = sem;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");
//	signal.semaphore = sem2;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");
}

void render_shared_resources::reset(){
	VkFence fences[] = {fence1,fence2};
//	VkSemaphoreSignalInfo signal{};
//	signal.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
//	signal.value = 0;
//	signal.semaphore = sem;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");
//	signal.semaphore = sem2;
//	if (vkSignalSemaphore(r_context->spinal_cord->device, &signal) != VK_SUCCESS)
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");

	if (vkResetFences(r_context->spinal_cord->device, 2, fences) != VK_SUCCESS)
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to signal semaphore!");
	queue.empty();
}

render_shared_resources::~render_shared_resources(){

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





