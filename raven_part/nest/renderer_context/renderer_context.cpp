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
#include <utility>
#include <string.h>
#include <algorithm>

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

void renderer_context::init_rsr_pool(){
	afsdfsd
}

void renderer_context::init_renderer_resource_storage(){
	sdada
}

void renderer_context::init_renderer_object_mapper(){
	sadasd
}

//primitive
void renderer_context::render_loop(harpy::raven_part::object_source& source, std::atomic_flag cond){
	size_t frame = 0;
	uint32_t image_index{};
	std::vector<std::pair<render_shared_resources*, uint32_t>> rendered_rsrs;
	std::pair<VkQueue, uint32_t> present_queue = spinal_cord->get_queue_supervisor().grab_presentation_queue((VkQueueFlags)0, connected_window_layout->surface);


	uint32_t effective_rsr_cnt = std::max((uint32_t)RSR_ABSURD_LIMIT, (uint32_t)queues_cnt, (uint32_t)(DEV_MEM_RENDER_PERSENTAGE * DEV_MEM_SIZE / RSR_GPU_MEM_COST));

	while(cond.test_and_set(std::memory_order_acquire)){
		//SPINLOCK
		rendered_rsrs.empty();
		while (source.consumed.test_and_set(std::memory_order_acquire)){}

		source.lock.lock();
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities = source.entities;
		size_t ptr_size = sizeof(human_part::ECS::Entity*);
		source.consumed.test_and_set();
		source.lock.unlock();
		uint32_t task_cnt = std::min((uint32_t)task_pool.size(), effective_rsr_cnt);
		uint32_t tgt_per_task = std::ceil((float)source.entities->size() / (float)task_cnt);
		spinal_cord->queue_supervisor.lock.lock();
		rsr_pool.lock.lock();
		for (uint32_t i = 0; i < task_cnt; ++i) {
			std::pair<VkQueue, uint32_t> vk_queue = spinal_cord->queue_supervisor.grab(VK_QUEUE_GRAPHICS_BIT);
			vkQueueWaitIdle(vk_queue.first);
			std::pair<render_shared_resources*, uint32_t> rsr = rsr_pool.lock_and_grab();
			rsr.wait();//vkWaitSemaphores(spinal_cord->device, )
			rsr.reset();//vkResetCommandBuffer(com_bufs[frame], 0);
				//reset sems
			//copy to subqueues
			std::memcpy(
					entities->data() + ptr_size * i * tgt_per_task,
					rsr.first->queue.data(),
					ptr_size * std::min(i * tgt_per_task,  (uint32_t)(rsr.first->queue.size()) - i * tgt_per_task));
			task_pool.start(render_task,{rsr,vk_queue});
			rendered_rsrs.push_back(rsr);
		}
		std::pair<VkQueue, uint32_t> blender_vk_queue = spinal_cord->queue_supervisor.grab(VK_QUEUE_GRAPHICS_BIT);
		vkQueueWaitIdle(blender_vk_queue.first);
		std::pair<render_shared_resources*, uint32_t> blender_rsr = rsr_pool.lock_and_grab();
		blender_rsr.wait();
		blender_rsr.reset();
		spinal_cord->queue_supervisor.lock.unlock();
		rsr_pool.lock.unlock();

		VkResult result = vkAcquireNextImageKHR(spinal_cord->device,
					swapchain, UINT32_MAX,
					swapchain.image_sems[frame], VK_NULL_HANDLE, &image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			swapchain.reinit();
			//rsr_pool.fb_resize();
			//for(auto& i : framebuffs)
			//	vkDestroyFramebuffer(vulkan_backend->get_vk_device(), i, nullptr);
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw utilities::harpy_little_error("failed to acquire swap chain image!");
		}

		blending(blender_rsr, blender_vk_queue, &rendered_rsrs, swapchain.fbs[image_index], swapchain.image_sems[frame]);

		present(present_queue,blender_rsr,image_index);


		frame = (frame + 1) % swapchain.image_sems.size();



	}
	vkDeviceWaitIdle(spinal_cord->device);
}

void renderer_context::render_task(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<VkQueue, uint32_t> vk_queue
		)
{

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


	// Привязка framebuffer
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = render_pass; // VkRenderPass, связанный с framebuffer
	renderPassBeginInfo.framebuffer = rsr.first->fb; // VkFramebuffer для привязки
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };

	vkBeginCommandBuffer(rsr.first->cmd, &beginInfo);
	vkCmdBeginRenderPass(rsr.first->cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	for object_mappings:
		vkCmdBindDescriptorSets(rsr.first->cmd);
		vkCmdPushConstants();
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);


		vkCmdBindPipeline(rsr.first->cmd);

		vkCmdDrawIndexed(rsr.first->cmd);

	vkCmdEndRenderPass(rsr.first->cmd);
	vkEndCommandBuffer(rsr.first->cmd);




	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//VkSemaphore waitSemaphores[] = {image_sems[frame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = VK_NULL_HANDLE;//waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &rsr.first->cmd;


	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &rsr.first->sem;

	if (vkQueueSubmit(vk_queue.first, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
	}



}


void renderer_context::blending(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<VkQueue, uint32_t> vk_queue,
		std::vector<std::pair<render_shared_resources*, uint32_t>>* rendered_rsrs,
		VkFramebuffer swapchain_fb,
		VkSemaphore image_sem
		)
{

	VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


		// Привязка framebuffer
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = render_pass; // VkRenderPass, связанный с framebuffer
		renderPassBeginInfo.framebuffer = swapchain_fb; // VkFramebuffer для привязки
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };

		vkBeginCommandBuffer(rsr.first->cmd, &beginInfo);
		vkCmdBeginRenderPass(rsr.first->cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


		vkCmdBindDescriptorSets(rsr.first->cmd);
		vkCmdPushConstants();
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);


		vkCmdBindPipeline(rsr.first->cmd);

		vkCmdDrawIndexed(rsr.first->cmd);

		vkCmdEndRenderPass(rsr.first->cmd);
		vkEndCommandBuffer(rsr.first->cmd);




		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		std::vector<VkSemaphore> waitSemaphores;
		waitSemaphores.reserve(rendered_rsrs->size() + 1);
		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			waitSemaphores.push_back((*rendered_rsrs)[i].first->sem);
		}
		waitSemaphores.push_back(image_sem);

		std::vector<VkSemaphore> signalSemaphores;
		waitSemaphores.reserve(rendered_rsrs->size() + 2);
		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			signalSemaphores.push_back((*rendered_rsrs)[i].first->sem2);
		}
		signalSemaphores.push_back(rsr.first->sem);
		signalSemaphores.push_back(rsr.first->sem2);

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &rsr.first->cmd;


		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		if (vkQueueSubmit(vk_queue.first, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
		}
}


void renderer_context::present(
		std::pair<VkQueue, uint32_t> present_queue,
		std::pair<render_shared_resources*, uint32_t> rsr,
		//VkSemaphore image_sem,
		uint32_t image_index){

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &rsr.first->sem;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &this->swapchain.chain;

	presentInfo.pImageIndices = &image_index;
	//should i use present queue?
	VkResult result = vkQueuePresentKHR(present_queue.first, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || connected_window_layout.get()->get_resize()) {
		connected_window_layout.get()->get_resize() = false;
		swapchain.reinit();
	} else if (result != VK_SUCCESS) {
		throw utilities::harpy_little_error("failed to present swap chain image!");
	}
}
