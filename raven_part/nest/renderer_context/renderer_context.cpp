/*
 * renderer_context.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#include <raven_part.h>
#include "renderer_context.h"
#include <windowing/base_window_layout.h>
#include <shaders/shader_module.h>
#include <ECS/Entity.h>
#include <ECS/Component.h>
#include <ECS/components/Renderer.h>
#include <ECS/components/Transform.h>
#include <resource_types/View.h>
#include <resource_types/Material.h>
#include <resource_types/Pipeline.h>
#include <resource_types/Shader.h>
#include <resource_types/Shape.h>
//#include <swapchain/swapchain.h>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <utility>
#include <string.h>
#include <algorithm>
#include <future>
#include <thread>
#include <iostream>
using namespace harpy::raven_part;
renderer_context::renderer_context(std::shared_ptr<vulkan_spinal_cord> cord, std::unique_ptr<base_window_layout> connected_window_layout){
	this->spinal_cord = cord;
	this->connected_window_layout = std::move(connected_window_layout);
	// = {this};
}

renderer_context::~renderer_context(){

	if (render_pass)
	vkDestroyRenderPass(this->spinal_cord->device, this->render_pass, nullptr);
//	if (pipeline_layout)
//	vkDestroyPipelineLayout(this->spinal_cord->device, this->pipeline_layout, nullptr);
	//if (descriptor_set_layout)
	//vkDestroyDescriptorSetLayout(this->spinal_cord->device, this->descriptor_set_layout, nullptr);


}

void renderer_context::init_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;


    VkAttachmentDescription depth_and_spencil_attachment{};
    depth_and_spencil_attachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
    depth_and_spencil_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    depth_and_spencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_and_spencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    depth_and_spencil_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_and_spencil_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depth_and_spencil_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_and_spencil_attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;


    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_GENERAL;

    VkAttachmentReference depth_and_spencil_attachment_ref{};
    depth_and_spencil_attachment_ref.attachment = 1;
    depth_and_spencil_attachment_ref.layout = VK_IMAGE_LAYOUT_GENERAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_and_spencil_attachment_ref;

    VkAttachmentDescription attachments[] = {color_attachment,depth_and_spencil_attachment};
    VkRenderPassCreateInfo render_pass_create_info{};
    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_create_info.attachmentCount = 2;
    render_pass_create_info.pAttachments = attachments;
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

    if(vkCreateRenderPass(this->spinal_cord->device, &render_pass_create_info, nullptr, &this->render_pass) != VK_SUCCESS)
        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Render pass hasn't been properly initialised");

//    VkDescriptorSetLayoutBinding uboLayoutBinding{};
//	uboLayoutBinding.binding = 0;
//	uboLayoutBinding.descriptorCount = 1;
//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	uboLayoutBinding.pImmutableSamplers = nullptr;
//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//	VkDescriptorSetLayoutCreateInfo layoutInfo{};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = 1;
//	layoutInfo.pBindings = &uboLayoutBinding;
//
//	if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo, nullptr, &this->descriptor_set_layout) != VK_SUCCESS) {
//		throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
//			"failed to create descriptor set layout!");
//	}

//	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo.setLayoutCount = 0;
//	pipelineLayoutInfo.pSetLayouts = nullptr;//&this->descriptor_set_layout;
//
//	if (vkCreatePipelineLayout(this->spinal_cord->device, &pipelineLayoutInfo, nullptr, &this->pipeline_layout) != VK_SUCCESS) {
//		 throw std::runtime_error("failed to create pipeline layout!");
//	}

}


void renderer_context::init_blender(){

		std::cout<<"init vkCreateRenderPass"<<std::endl;
		VkDescriptorSetLayoutBinding colorLayoutBinding{};
		colorLayoutBinding.binding = 0;
		//colorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		colorLayoutBinding.descriptorCount = effective_rsr_cnt;
		colorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		colorLayoutBinding.pImmutableSamplers = nullptr;
		colorLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;



		VkDescriptorSetLayoutBinding depthLayoutBinding{};
		depthLayoutBinding.binding = 0;
		//depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		depthLayoutBinding.descriptorCount = effective_rsr_cnt;
		depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthLayoutBinding.pImmutableSamplers = nullptr;
		depthLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding outLayoutBinding{};
		outLayoutBinding.binding = 0;
		//depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		outLayoutBinding.descriptorCount = 1;
		outLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		outLayoutBinding.pImmutableSamplers = nullptr;
		outLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
			VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
			VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;//VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT |

		VkDescriptorBindingFlags bindingFlagsArr[] = {bindingFlags};
		VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo1 = {};
		extendedInfo1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		extendedInfo1.bindingCount = 1;
		extendedInfo1.pBindingFlags = bindingFlagsArr;

		VkDescriptorSetLayoutCreateInfo layoutInfo1{};
		layoutInfo1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo1.bindingCount = 1;
		layoutInfo1.pBindings = &colorLayoutBinding;
		layoutInfo1.pNext = &extendedInfo1;


		VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo2 = {};
		extendedInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		extendedInfo2.bindingCount = 1;
		extendedInfo2.pBindingFlags = bindingFlagsArr;

		VkDescriptorSetLayoutCreateInfo layoutInfo2{};
		layoutInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo2.bindingCount = 1;
		layoutInfo2.pBindings = &depthLayoutBinding;
		layoutInfo2.pNext = &extendedInfo2;

		VkDescriptorSetLayoutCreateInfo layoutInfo3{};
		layoutInfo3.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo3.bindingCount = 1;
		layoutInfo3.pBindings = &outLayoutBinding;


		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo1, nullptr, &this->blender_descriptor_set_layout1) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor set layout!");
		}
		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo2, nullptr, &this->blender_descriptor_set_layout2) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor set layout!");
		}

		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo3, nullptr, &this->blender_descriptor_set_layout_out) != VK_SUCCESS) {
				throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
					"failed to create descriptor set layout!");
			}
		std::cout<<"init vkCreateDescriptorSetLayout"<<std::endl;
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(blender_push_constants);

		VkDescriptorSetLayout layouts[] = {blender_descriptor_set_layout1,blender_descriptor_set_layout2, blender_descriptor_set_layout_out};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 3;
		pipelineLayoutInfo.pSetLayouts = layouts;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->spinal_cord->device, &pipelineLayoutInfo, nullptr, &this->blender_pipeline_layout) != VK_SUCCESS) {
			 throw std::runtime_error("failed to create pipeline layout!");
		}
		std::cout<<"init vkCreatePipelineLayout"<<std::endl;



		shaders::shader_module fragment{spinal_cord->device};
		fragment.init(SHADER_PATH_BLENDER_FRAGMENT);
		std::cout<<"init shaders succ"<<std::endl;


		VkPipelineShaderStageCreateInfo create_fragment_info{};
		create_fragment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_fragment_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		create_fragment_info.module = fragment.module;
		create_fragment_info.pName = "main";


		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.stage = create_fragment_info;
		pipelineInfo.layout = blender_pipeline_layout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateComputePipelines(this->spinal_cord->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &blender_pipeline) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create graphics pipeline!");
		}
		std::cout<<"init vkCreateGraphicsPipelines"<<std::endl;
		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_TRUE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		vkCreateSampler(this->spinal_cord->device, &samplerCreateInfo, nullptr, &blender_sampler);
		std::cout<<"blender inited succ"<<std::endl;

}




void renderer_context::init_swapchain(){
	swapchain.init();
	//connected_window_layout.init_all(instance);
}

void renderer_context::init_rsr_pool(){
	rsr_pool.init(effective_rsr_cnt);
}

void renderer_context::init_renderer_resource_storage(){
	//sdada
}

void renderer_context::init_renderer_object_mapper(){
	//sadasd
}
void render_task_fake(renderer_context* ctx, std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		uint32_t view_id){
	//std::cout<<"probe render_task_fake"<<std::endl;
	ctx->render_task(rsr,vk_queue,view_id);
}
//primitive
void renderer_context::render_loop(std::shared_ptr<harpy::raven_part::scene_source> source, std::atomic_flag* cond){
	//std::cout<<"probe0"<<std::endl;
	size_t frame = 0;
	uint32_t image_index{};
	uint32_t task_cnt = std::min((uint32_t)std::thread::hardware_concurrency(), effective_rsr_cnt);
	std::vector<std::pair<render_shared_resources*, uint32_t>> rendered_rsrs{};
	std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> present_queue = spinal_cord->get_queue_supervisor().grab_presentation_queue((VkQueueFlags)0, connected_window_layout->surface);
	std::vector<std::thread> threads{};

	//std::cout<<"probe1"<<std::endl;
	while(cond->test_and_set(std::memory_order_acquire)){
		//should i waip present queue? in vulkan-guide there is no vkQueueWaitIdle(present_queue);

		//SPINLOCK
		rendered_rsrs.clear();
		threads.clear();
		while (source->consumed.test_and_set(std::memory_order_acquire)){}
		//std::cout<<"probe2"<<std::endl;
		source->lock.lock();
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities = source->entities;
		uint32_t view_id = source->view_id;
		size_t ptr_size = sizeof(human_part::ECS::Entity*);
		source->consumed.test_and_set();
		source->lock.unlock();
		//std::cout<<"probe3"<<std::endl;
		if (task_cnt == 0) continue;
		uint32_t tgt_per_task = std::ceil((float)source->entities->size() / (float)task_cnt);
		//if (tgt_per_task == 0) continue;
		//std::cout<<"probe3.1"<<std::endl;

		//std::cout<<"probe3.2"<<std::endl;
		mapper.lock.lock();

		renderer_context * context_ptr = this;
		//std::cout<<"probe4"<<std::endl;
		//rendered_rsrs.resize(task_cnt);
		for (uint32_t i = 0; i < task_cnt; ++i) {
			int32_t cnt = std::max(std::min((int32_t)tgt_per_task,  (int32_t)(entities->size()) - (int32_t)(i * tgt_per_task)),(int32_t)0);
			if(!cnt) continue;
			//std::cout<<"probe4.0 "<<i<<std::endl;
			std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue = spinal_cord->queue_supervisor.lock_grab(VK_QUEUE_GRAPHICS_BIT);
			std::cout<<"got queue "<<vk_queue.second<<std::endl;
			//std::cout<<"probe4.0.1"<<std::endl;
			if (vkQueueWaitIdle(vk_queue.first.first) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to wait queue!");
			//std::cout<<"probe4.1"<<std::endl;
			std::pair<render_shared_resources*, uint32_t> rsr = rsr_pool.lock_grab();
			//std::cout<<"probe4.1.1"<<std::endl;
			rsr.first->wait();
			//std::cout<<"probe4.1.2"<<std::endl;
			rsr.first->reset();
			//std::cout<<"probe4.2"<<std::endl;
				//reset sems
			//copy to subqueues



			//std::cout<<"probe4.3 "<<cnt<<std::endl;
			rsr.first->queue.resize(cnt);
			//std::cout<<"probe4.4"<<std::endl;
			for (int k = 0; k < cnt; ++k) {
				//std::cout<<"probe4.5"<<std::endl;
				rsr.first->queue[k].first = (*entities)[i * tgt_per_task + k];
				//std::cout<<"probe4.6"<<std::endl;
				harpy::human_part::ECS::Entity* e = ((*entities)[i * tgt_per_task + k]);
				//std::cout<<"probe4.6.1"<<std::endl;
				std::vector<harpy::human_part::ECS::Component*> comps = e->get_components_by_name(harpy::human_part::ECS::Renderer::name);
				//std::cout<<"probe4.7"<<std::endl;
				uint32_t mappings_id = dynamic_cast<harpy::human_part::ECS::Renderer*>(comps[0])->mapping_id;
				//std::cout<<"probe4.8"<<std::endl;
				rsr.first->queue[k].second = mapper.mappings[mappings_id];
			}
			//std::cout<<"probe4.9"<<std::endl;
			/*std::memcpy(
					entities->data() + ptr_size * i * tgt_per_task,
					rsr.first->queue.data(),
					ptr_size * cnt);*/
//			std::async(std::launch::async, [rsr, vk_queue, context_ptr, view_id]() {
//				context_ptr->render_task(rsr, vk_queue, view_id);
//			    });
			threads.push_back(std::thread(render_task_fake,context_ptr,rsr, vk_queue, view_id));
			//std::cout<<"probe4.10"<<std::endl;

			rendered_rsrs.push_back(rsr);
			//rendered_rsrs[i] = rsr;
		}
		//std::cout<<"probe5"<<std::endl;
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> blender_vk_queue = spinal_cord->queue_supervisor.lock_grab(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
		std::cout<<"got blender queue "<<blender_vk_queue.second<<std::endl;
		std::cout<<"probe5.1"<<std::endl;
		std::cout<<"wait blender queue "<<blender_vk_queue.second<<std::endl;
		if (vkQueueWaitIdle(blender_vk_queue.first.first) != VK_SUCCESS)
						throw utilities::harpy_little_error("failed to wait blender queue!");
		std::cout<<"probe5.1.1 waited"<<std::endl;
		std::pair<render_shared_resources*, uint32_t> blender_rsr = rsr_pool.lock_grab();
		blender_rsr.first->wait();
		blender_rsr.first->reset();
		mapper.lock.unlock();
		//std::cout<<"probe6"<<std::endl;

		VkResult result = vkAcquireNextImageKHR(spinal_cord->device,
					swapchain, UINT32_MAX,
					swapchain.image_sems[frame], VK_NULL_HANDLE, &image_index);
		//std::cout<<"probe7"<<std::endl;
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			swapchain.reinit();
			//rsr_pool.fb_resize();
			//for(auto& i : framebuffs)
			//	vkDestroyFramebuffer(vulkan_backend->get_vk_device(), i, nullptr);
			return;
		}
		//std::cout<<"probe8"<<std::endl;
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw utilities::harpy_little_error("failed to acquire swap chain image!");
		}
		//std::cout<<"probe9"<<std::endl;
		for (int i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}

		blending(blender_rsr, blender_vk_queue, &rendered_rsrs, swapchain.images[image_index], swapchain.image_views[image_index], swapchain.image_sems[frame]);
		//std::cout<<"probe10"<<std::endl;
		present(present_queue,blender_rsr,image_index);
		//std::cout<<"probe11"<<std::endl;

		for (int i = 0; i < rendered_rsrs.size(); ++i) {
			rsr_pool.lock_free(rendered_rsrs[i].second);
		}
		rsr_pool.lock_free(blender_rsr.second);


		spinal_cord->queue_supervisor.lock_free(blender_vk_queue.second);

		frame = (frame + 1) % swapchain.image_sems.size();
		std::cout<<"probe12"<<std::endl;


	}
	vkDeviceWaitIdle(spinal_cord->device);

	spinal_cord->queue_supervisor.lock_free(present_queue.second);
}

void renderer_context::render_task(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		uint32_t view_id
		)
{
	//std::cout<<"probe render_task0"<<std::endl;
	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to reset cmd RSR!");
	//std::cout<<"probe render_task1"<<std::endl;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkClearValue clears[] = {clear_color,clear_depth_stencil};
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = render_pass;
	renderPassBeginInfo.framebuffer = rsr.first->fb;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };
	renderPassBeginInfo.pClearValues = clears;
	renderPassBeginInfo.clearValueCount = 2;

	if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to begin command buffer!");
	//std::cout<<"probe render_task2"<<std::endl;
	vkCmdBeginRenderPass(vk_queue.first.second, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

	for (uint32_t i = 0; i < rsr.first->queue.size(); i++){

		std::cout<<"render task: "<<i<<std::endl;
		std::pair<harpy::human_part::ECS::Entity*, renderer_mappings> pr = rsr.first->queue[i];
		//std::cout<<"probe render_task2.0.1"<<std::endl;

		resource_types::View view = storage.views[view_id];
		//std::cout<<"probe render_task2.0.2"<<std::endl;
		//resource_types::Pipeline pipeline = storage.pipelines[pr.second.pipeline_id];
		resource_types::Shape object = storage.shapes[pr.second.shape_id];
		resource_types::Material material = storage.materials[pr.second.material_id];
		//std::cout<<"probe render_task2.1"<<std::endl;
//=====================
//    VIEW
//=====================
	   vkCmdSetViewport(vk_queue.first.second, 0, 1, &view.viewport);

	   vkCmdSetScissor(vk_queue.first.second, 0, 1, &view.scissor);
	   //std::cout<<"probe render_task2.2"<<std::endl;
//=====================
//    OBJECT
//=====================
	   VkDeviceSize offsets[] = {0};
	   vkCmdBindVertexBuffers(vk_queue.first.second, 0, 1, &object.vertexBuffer, offsets);

	   vkCmdBindIndexBuffer(vk_queue.first.second, object.indexBuffer, 0, object.indexType);
	   //std::cout<<"probe render_task2.3"<<std::endl;

//=====================
//    CAMERA TRANSPOSE
//=====================

//	   vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, view.cameraGraphicsPipeline);
//
//	   vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &view.desc_set, 0, nullptr);
//
//	   vkCmdPushConstants(vk_queue.first.second,pipeline_layout, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, sizeof(view.cameraPushConstants), &view.cameraPushConstants);
//
//	   vkCmdDrawIndexed(vk_queue.first.second, object.indices_size, 1, 0, 0, 0);


//=====================
//    MATERIAL
//=====================
	   vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
	   if(material.desc_set)
	   vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipelineLayout, 0, 1, &material.desc_set, 0, nullptr);
	   //std::cout<<"probe render_task2.4"<<std::endl;
//=====================
//    DRAW
//=====================

	   vkCmdDrawIndexed(vk_queue.first.second, object.indices_size, 1, 0, 0, 0);
	   //std::cout<<"probe render_task2.5"<<std::endl;
//=====================
//    EFFECT
//=====================

//	   vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);
//
//	   vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &pipeline.desc_set, 0, nullptr);




	}
	//std::cout<<"probe render_task3"<<std::endl;
	vkCmdEndRenderPass(vk_queue.first.second);
	//std::cout<<"probe render_task4"<<std::endl;

	if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to end command buffer!");
	//std::cout<<"probe render_task5"<<std::endl;
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = VK_NULL_HANDLE;//waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vk_queue.first.second;


	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &rsr.first->sem;
	//std::cout<<"probe render_task6"<<std::endl;
	if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, rsr.first->fence1) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
	}
	//std::cout<<"probe render_task7"<<std::endl;
	rsr.first->color_image_layout = VK_IMAGE_LAYOUT_GENERAL;
	rsr.first->depth_and_stencil_image_layout = VK_IMAGE_LAYOUT_GENERAL;
	//std::cout<<"probe render_task8"<<std::endl;
	//rsr_pool.lock_free(rsr.second);
	//std::cout<<"probe render_task9"<<std::endl;
	spinal_cord->queue_supervisor.lock_free(vk_queue.second);
	std::cout<<"probe render_task10"<<std::endl;

}


void renderer_context::blending(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::vector<std::pair<render_shared_resources*, uint32_t>>* rendered_rsrs,
		VkImage swapchain_image,
		VkImageView swapchain_image_view,
		VkSemaphore image_sem
		)
{
	//std::cout<<"probe blending0"<<std::endl;
	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to reset cmd RSR!");
	//std::cout<<"probe blending1"<<std::endl;
	VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


		if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin command buffer!");
		//std::cout<<"probe blending2"<<std::endl;
		if (rsr.first->blender_set1){
			VkDescriptorSet sets[] = {rsr.first->blender_set1,rsr.first->blender_set2,rsr.first->blender_set_out};
			if (rsr.first->blender_set1 && vkFreeDescriptorSets(spinal_cord->device, rsr.first->blender_desc_pool, 3, sets) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to free descriptor set!");
		}
		//std::cout<<"probe blending4"<<std::endl;

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = rsr.first->blender_desc_pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &blender_descriptor_set_layout_out;

		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo, &rsr.first->blender_set_out) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");


		//std::cout<<"probe blending5"<<std::endl;
		VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountInfo{};
		uint32_t sizes[] = {rendered_rsrs->size(),rendered_rsrs->size()};
		variableDescriptorCountInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		variableDescriptorCountInfo.descriptorSetCount = 2;
		variableDescriptorCountInfo.pDescriptorCounts = sizes;

		VkDescriptorSetLayout layouts[] = {blender_descriptor_set_layout1, blender_descriptor_set_layout2};
		allocInfo.descriptorSetCount = 2;
		allocInfo.pSetLayouts = layouts;
		allocInfo.pNext = &variableDescriptorCountInfo;

		VkDescriptorSet sets2[2] = {rsr.first->blender_set1,rsr.first->blender_set2};

		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo, sets2) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");
		rsr.first->blender_set1 = sets2[0];
		rsr.first->blender_set2 = sets2[1];
		//std::cout<<"probe blending6"<<std::endl;
//		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo2, &blender_set2) != VK_SUCCESS)
//			throw utilities::harpy_little_error("failed to allocate descriptor set!");
//

		std::vector<VkDescriptorImageInfo> color_buffer(rendered_rsrs->size());
		std::vector<VkDescriptorImageInfo> depth_buffer(rendered_rsrs->size());
		//std::cout<<"probe blending7"<<std::endl;

		for (uint32_t i = 0; i < rendered_rsrs->size(); i++) {
			//std::cout<<"probe blending7.0"<<std::endl;
			color_buffer[i].imageView = (*rendered_rsrs)[i].first->color_image_view;
			color_buffer[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			color_buffer[i].sampler = blender_sampler;
			//std::cout<<"probe blending7.1"<<std::endl;
			depth_buffer[i].imageView = (*rendered_rsrs)[i].first->depth_image_view;
			depth_buffer[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			depth_buffer[i].sampler = blender_sampler;
			//std::cout<<"probe blending7.2"<<std::endl;
		}
		//std::cout<<"probe blending7.1"<<std::endl;
		VkDescriptorImageInfo out_buffer{};
		out_buffer.imageView = rsr.first->color_image_view;
		out_buffer.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		//out_buffer.sampler = blender_sampler;


		VkWriteDescriptorSet color_buffer_write_descriptor_set = {};
		color_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		color_buffer_write_descriptor_set.dstSet = rsr.first->blender_set1;
		color_buffer_write_descriptor_set.dstBinding = 0;
		color_buffer_write_descriptor_set.dstArrayElement = 0;
		color_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		color_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		color_buffer_write_descriptor_set.pImageInfo = color_buffer.data();


		VkWriteDescriptorSet depth_and_stencil_buffer_write_descriptor_set = {};
		depth_and_stencil_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depth_and_stencil_buffer_write_descriptor_set.dstSet = rsr.first->blender_set2;
		depth_and_stencil_buffer_write_descriptor_set.dstBinding = 0;
		depth_and_stencil_buffer_write_descriptor_set.dstArrayElement = 0;
		depth_and_stencil_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depth_and_stencil_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		depth_and_stencil_buffer_write_descriptor_set.pImageInfo = depth_buffer.data();


		VkWriteDescriptorSet out_descriptor_set = {};
		out_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		out_descriptor_set.dstSet = rsr.first->blender_set_out;
		out_descriptor_set.dstBinding = 0;
		out_descriptor_set.dstArrayElement = 0;
		out_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		out_descriptor_set.descriptorCount = 1;
		out_descriptor_set.pImageInfo = &out_buffer;

		VkWriteDescriptorSet writeDescriptorSets[] = {color_buffer_write_descriptor_set, depth_and_stencil_buffer_write_descriptor_set,out_descriptor_set};
		//std::cout<<"probe blending8"<<std::endl;
		vkUpdateDescriptorSets(spinal_cord->device, 3, writeDescriptorSets, 0, nullptr);

		VkDescriptorSet sets3[] = {rsr.first->blender_set1, rsr.first->blender_set2, rsr.first->blender_set_out};
		//std::cout<<"probe blending9"<<std::endl;
		vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_COMPUTE, blender_pipeline_layout, 0, 3, sets3, 0, nullptr);
		//std::cout<<"probe blending10"<<std::endl;

		blender_push_constants.layers_cnt = rendered_rsrs->size();
		//std::cout<<"probe blending11"<<std::endl;
		vkCmdPushConstants(vk_queue.first.second, blender_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(blender_push_constants), &blender_push_constants);

		//std::cout<<"probe blending12"<<std::endl;
		vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_COMPUTE, blender_pipeline);

		VkImageMemoryBarrier imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = 1;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.layerCount = 1;

		if (rsr.first->color_image_layout != VK_IMAGE_LAYOUT_GENERAL){
			imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageBarrier.image = rsr.first->color_image;

			vkCmdPipelineBarrier(
				vk_queue.first.second,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageBarrier
			);


		}


		//std::cout<<"probe blending13"<<std::endl;
		vkCmdDispatch(vk_queue.first.second,swapchain.extent.width,swapchain.extent.height,1);




		imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier.image = swapchain_image;

		vkCmdPipelineBarrier(
			vk_queue.first.second,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageBarrier
		);

		VkImageCopy imageCopy{};
		imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopy.srcSubresource.layerCount = 1;
		imageCopy.srcOffset = { 0, 0, 0 };
		imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopy.dstSubresource.layerCount = 1;
		imageCopy.dstOffset = { 0, 0, 0 };
		imageCopy.extent = { swapchain.extent.width, swapchain.extent.height, 1 };


		vkCmdCopyImage(
			vk_queue.first.second,
		    rsr.first->color_image,
			VK_IMAGE_LAYOUT_GENERAL,
			swapchain_image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		    1,
		    &imageCopy
		);


		imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageBarrier.image = swapchain_image;

		vkCmdPipelineBarrier(
			vk_queue.first.second,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &imageBarrier
		);


		//std::cout<<"probe blending15"<<std::endl;
		if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to end command buffer!");


		//std::cout<<"probe blending16"<<std::endl;
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		std::vector<VkSemaphore> waitSemaphores;
		std::vector<VkPipelineStageFlags> waitStages;
		waitSemaphores.reserve(rendered_rsrs->size() + 1);
		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			waitSemaphores.push_back((*rendered_rsrs)[i].first->sem);
			waitStages.push_back(VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
		}
		waitSemaphores.push_back(image_sem);
		waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		std::vector<VkSemaphore> signalSemaphores;

		signalSemaphores.reserve(rendered_rsrs->size() + 2);
//		for (int i = 0; i < rendered_rsrs->size(); ++i) {
//			signalSemaphores.push_back((*rendered_rsrs)[i].first->sem2);
//		}
		signalSemaphores.push_back(rsr.first->sem);
		//signalSemaphores.push_back(rsr.first->sem2);

		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vk_queue.first.second;

		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		//std::cout<<"probe blending17"<<std::endl;
		if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, rsr.first->fence1) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
		}
		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			(*rendered_rsrs)[i].first->blender_fence = rsr.first->fence1;
		}
		rsr.first->blender_fence = nullptr;
		rsr.first->color_image_layout = VK_IMAGE_LAYOUT_GENERAL;

		std::cout<<"probe blending18"<<std::endl;
}


void renderer_context::present(
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> present_queue,
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

	VkResult result = vkQueuePresentKHR(present_queue.first.first, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || connected_window_layout.get()->get_resize()) {
		connected_window_layout.get()->get_resize() = false;
		swapchain.reinit();
	} else if (result != VK_SUCCESS) {
		throw utilities::harpy_little_error("failed to present swap chain image!");
	}
}

void renderer_context::init(){
	std::cout<<"renderer_context initialising stated"<<std::endl;
	connected_window_layout->init(this);
	std::cout<<"init connected_window_layout succ"<<std::endl;
	init_swapchain();
	std::cout<<"init_swapchain succ"<<std::endl;
	init_render_pass();
	std::cout<<"init_render_pass succ"<<std::endl;
//	init_descriptor_pool();
//	std::cout<<"init_descriptor_pool succ"<<std::endl;
	init_rsr_pool();
	std::cout<<"init_rsr_pool succ"<<std::endl;
	init_renderer_resource_storage();
	std::cout<<"init_renderer_resource_storage succ"<<std::endl;
	init_renderer_object_mapper();
	std::cout<<"init_renderer_object_mapper succ"<<std::endl;
	init_blender();
	std::cout<<"init_blender succ"<<std::endl;

	swapchain.init_image_views();

}



