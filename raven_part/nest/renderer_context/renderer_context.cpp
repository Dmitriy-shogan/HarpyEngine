/*
 * renderer_context.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#include <../raven_part.h>>
#include "renderer_context.h"
#include <windowing/base_window_layout.h>
//#include <swapchain/swapchain.h>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
using namespace harpy::raven_part;
renderer_context::renderer_context(std::shared_ptr<vulkan_spinal_cord> cord, std::unique_ptr<base_window_layout> connected_window_layout) : swapchain(this){
			this->spinal_cord = cord;
			this->connected_window_layout = std::move(connected_window_layout);
			// = {this};
			init_swapchain();
			init_render_pass();
			init_layouts();
			init_rsr_pool();
			init_descriptor_pool();
		}

renderer_context::~renderer_context(){

			delete &rsr_pool;
			if (render_pass)
			vkDestroyRenderPass(this->spinal_cord->device, this->render_pass, nullptr);
			if (pipeline_layout)
			vkDestroyPipelineLayout(this->spinal_cord->device, this->pipeline_layout, nullptr);
			if (descriptor_set_layout)
			vkDestroyDescriptorSetLayout(this->spinal_cord->device, this->descriptor_set_layout, nullptr);
			delete &swapchain;

		}

void renderer_context::init_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = swapchain.surface_format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attachment_ref{};
    attachment_ref.attachment = 0;
    attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachment_ref;


    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 1;
    render_pass_create_info.pAttachments = &color_attachment;
    render_pass_create_info.subpassCount = 1;
    render_pass_create_info.pSubpasses = &subpass;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;

    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = &dependency;

    if(vkCreateRenderPass(this->spinal_cord->device, &render_pass_create_info, nullptr, &this->render_pass) != VK_SUCCESS){
        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Render pass hasn't been properly initialised");
    }
}

void renderer_context::init_layouts(){

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo, nullptr, &this->descriptor_set_layout) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
			"failed to create descriptor set layout!");
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &this->descriptor_set_layout;

	if (vkCreatePipelineLayout(this->spinal_cord->device, &pipelineLayoutInfo, nullptr, &this->pipeline_layout) != VK_SUCCESS) {
	     throw std::runtime_error("failed to create pipeline layout!");
	}
}

void renderer_context::init_descriptor_pool()
	{

	VkDescriptorPoolSize pool_size{};
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &pool_size;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(this->spinal_cord->device, &poolInfo, nullptr, &desc_pool) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor pool!");
	}

		//std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
//		VkDescriptorSetAllocateInfo allocInfo{};
//		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		allocInfo.descriptorPool = pool.get_vk_descriptor_pool();
//		allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
//		allocInfo.pSetLayouts = layouts.data();
//
//		sets.resize(MAX_FRAMES_IN_FLIGHT);
//		if (vkAllocateDescriptorSets(device, &allocInfo, sets.data()) != VK_SUCCESS) {
//			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
//				"failed to allocate descriptor sets!");
//		}
	}

//void renderer_conext::descriptor_pool_populate(std::vector<buffers::uniform_buffer>& buffers)
//{
//	for(int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
//		VkDescriptorBufferInfo bufferInfo{};
//		bufferInfo.offset = 0;
//		bufferInfo.range = sizeof(ubo);
//		bufferInfo.buffer = buffers[i].get_vk_buffer();
//
//		VkWriteDescriptorSet descriptorWrite{};
//		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		descriptorWrite.dstSet = sets[i];
//		descriptorWrite.dstBinding = 0;
//		descriptorWrite.dstArrayElement = 0;
//		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		descriptorWrite.descriptorCount = 1;
//		descriptorWrite.pBufferInfo = &bufferInfo;
//
//		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
//	}
//}

void renderer_context::init_swapchain(){
	swapchain.init();
	//connected_window_layout.init_all(instance);
}


void renderer_context::render_loop(harpy::raven_part::object_source& source, std::atomic_bool cond){
	while(cond){
		source.lock.lock();
		std::shared_ptr<std::vector<human_part::ECS::Entity>> entities = source.entities;
		source.consumed = true;
		source.lock.unlock();

		for (int i = 0; i < rsr_pool.size(); ++i) {
			render_task
		}

		VkResult result = vkAcquireNextImageKHR(spinal_cord->device,
					*swapchain, UINT64_MAX,
					image_sems[frame], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			chain->reinit();
			for(auto& i : framebuffs)
				vkDestroyFramebuffer(vulkan_backend->get_vk_device(), i, nullptr);
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw utilities::harpy_little_error("failed to acquire swap chain image!");
		}



	}
}

void renderer_context::render_task(
		std::queue<harpy::human_part::ECS::Entity*>* queue,
		nest::render_shared_resources rsr,
		VkQueue * vk_queue
		)
{
	//vkWaitForFences(vulkan_backend->get_vk_device(), 1, &fences_in_flight[frame], VK_TRUE, UINT64_MAX);

	uint32_t image_index{};




	//object.rotate(15, 1, 0, 0);
	nest::ubo ub{};
	ub.model = object.get_model();
	ub.view = camera.get_view();
	ub.projection = nest::projection;
	object.get_uniform_buffers()[frame].set_ubo(ub);

	vkResetFences(vulkan_backend->get_vk_device(), 1, &fences_in_flight[frame]);

	vkResetCommandBuffer(com_bufs[frame], 0);

	controller.connect(com_bufs[frame], image_index);
	controller.draw(object.get_vertex_buffer(), object.get_index_buffer(), desc, frame);
	controller.disconnect();


	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = {image_sems[frame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &com_bufs[frame].get_vk_command_buffer();

	VkSemaphore signalSemaphores[] = {finish_sems[frame]};
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vulkan_backend->get_vk_present_queue(), 1, &submitInfo, fences_in_flight[frame]) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {*chain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &image_index;

	result = vkQueuePresentKHR(vulkan_backend->get_vk_graphics_queue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.get_resize()) {
		window.get_resize() = false;
		chain->reinit();
	} else if (result != VK_SUCCESS) {
		throw utilities::harpy_little_error("failed to present swap chain image!");
	}

	frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
}


