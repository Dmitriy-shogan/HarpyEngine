/*
 * render_shared_resources.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */



#include "render_shared_resources.h"
#include <renderer_context/renderer_context.h>
#include <swapchain/swapchain.h>


void render_shared_resources::init_framebuffer(){

	VkFramebufferCreateInfo framebuffer_info{};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.renderPass = r_context->render_pass;
	framebuffer_info.attachmentCount = 1;
	framebuffer_info.width = r_context->swapchain.get_vk_extent().width;
	framebuffer_info.height = r_context->swapchain.get_vk_extent().height;
	framebuffer_info.layers = 1;


	framebuffer_info.pAttachments = &image_view;

	if (vkCreateFramebuffer(r_context->spinal_cord->device, &framebuffer_info, nullptr, &this->fb) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init framebuffer!");
	}

}


render_shared_resources::~render_shared_resources(){
	if(cmd)
	vkFreeCommandBuffers(r_context->spinal_cord->device, *(this->command_pool), 1, &(this->cmd));
	if(fb)
	vkDestroyFramebuffer(r_context->spinal_cord->device,this->fb,nullptr);
	if(image_view)
	vkDestroyImageView(r_context->spinal_cord->device,this->image_view,nullptr);
	if(image)
	vkDestroyImage(r_context->spinal_cord->device,this->image,nullptr);
}


