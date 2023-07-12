/*
 * renderer_context.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#include <../raven_part.h>>
#include "renderer_context.h"
#include <windowing/base_window_layout.h>
#include <shaders/shader_module.h>
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
	init_rsr_pool();
	init_descriptor_pool();
	init_blender();
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
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription depth_and_spencil_attachment{};
    depth_and_spencil_attachment.format = swapchain.surface_format.format;
    depth_and_spencil_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    depth_and_spencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_and_spencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    depth_and_spencil_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_and_spencil_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depth_and_spencil_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_and_spencil_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_and_spencil_attachment_ref{};
    depth_and_spencil_attachment_ref.attachment = 1;
    depth_and_spencil_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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


void renderer_context::init_blender(){
	VkAttachmentDescription color_attachment{};
	    color_attachment.format = swapchain.surface_format.format;
	    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

	    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	    color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;



	    VkAttachmentReference color_attachment_ref{};
	    color_attachment_ref.attachment = 0;
	    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	    VkSubpassDescription subpass{};
	    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	    subpass.colorAttachmentCount = 1;
	    subpass.pColorAttachments = &color_attachment_ref;



	    VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;

		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	    VkRenderPassCreateInfo render_pass_create_info{};
	    render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	    render_pass_create_info.dependencyCount = 1;
	    render_pass_create_info.pDependencies = &dependency;
	    render_pass_create_info.subpassCount = 1;
	    render_pass_create_info.pSubpasses = &subpass;


	    VkAttachmentDescription attachments[] = {color_attachment};
		render_pass_create_info.attachmentCount = 1;
		render_pass_create_info.pAttachments = attachments;

		if(vkCreateRenderPass(this->spinal_cord->device, &render_pass_create_info, nullptr, &this->blender_render_pass) != VK_SUCCESS)
		        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Blender's Render pass hasn't been properly initialised");


		VkDescriptorSetLayoutBinding colorLayoutBinding{};
		colorLayoutBinding.binding = 0;
		colorLayoutBinding.descriptorCount = 1;
		colorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		colorLayoutBinding.pImmutableSamplers = nullptr;
		colorLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;



		VkDescriptorSetLayoutBinding depthLayoutBinding{};
		depthLayoutBinding.binding = 1;
		depthLayoutBinding.descriptorCount = 1;
		depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		depthLayoutBinding.pImmutableSamplers = nullptr;
		depthLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


		VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
			VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
			VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT |
			VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;

		VkDescriptorBindingFlags bindingFlagsArr[] = {bindingFlags,bindingFlags};
		VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo = {};
		extendedInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		extendedInfo.bindingCount = 2;
		extendedInfo.pBindingFlags = bindingFlagsArr;

		VkDescriptorSetLayoutBinding bindings[] = {colorLayoutBinding,depthLayoutBinding};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 2;
		layoutInfo.pBindings = bindings;
		layoutInfo.pNext = extendedInfo;

		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo, nullptr, &this->blender_descriptor_set_layout) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor set layout!");
		}

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(blender_push_constants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &this->descriptor_set_layout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->spinal_cord->device, &pipelineLayoutInfo, nullptr, &this->blender_pipeline_layout) != VK_SUCCESS) {
			 throw std::runtime_error("failed to create pipeline layout!");
		}

		VkDescriptorSetLayout layouts[] = {blender_descriptor_set_layout};




		VkViewport blender_dynamic_viewport;
		VkRect2D blender_dynamic_scissors;

		VkClearValue clear_color{{{0.0f,0.0f,0.0f,1.0f}}};

		shaders::shader_module vertex{spinal_cord->device};
		shaders::shader_module fragment{spinal_cord->device};
		vertex.init(SHADER_PATH_BLENDER_VERTEX_DUMMY);
		fragment.init(SHADER_PATH_BLENDER_FRAGMENT);

		VkPipelineShaderStageCreateInfo create_vertex_info{};
		create_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_vertex_info.stage = VK_SHADER_STAGE_VERTEX_BIT;

		create_vertex_info.module = vertex;
		create_vertex_info.pName = "main";

		VkPipelineShaderStageCreateInfo create_fragment_info{};
		create_fragment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_fragment_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		create_fragment_info.module = fragment;
		create_fragment_info.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {create_vertex_info, create_fragment_info};
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto bindingDescription = get_binding_description();
		auto attributeDescriptions = get_attributes_descriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE; //IDK should i use it

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_TRUE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;



		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_FALSE;
		depthStencil.depthWriteEnable = VK_FALSE;
		//depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.stencilTestEnable = VK_FALSE;


		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();




		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = blender_pipeline_layout;
		pipelineInfo.renderPass = blender_render_pass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(this->spinal_cord->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &blender_pipeline) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create graphics pipeline!");
		}

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_TRUE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		vkCreateSampler(this->spinal_cord->device, &samplerCreateInfo, nullptr, &blender_sampler);


}

void renderer_context::init_descriptor_pool()
	{

	VkDescriptorPoolSize pool_size{};
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = static_cast<uint32_t>(1);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &pool_size;
	poolInfo.maxSets = static_cast<uint32_t>(1);

	if (vkCreateDescriptorPool(this->spinal_cord->device, &poolInfo, nullptr, &blender_desc_pool) != VK_SUCCESS) {
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
		//should i waip present queue? in vulkan-guide there is no vkQueueWaitIdle(present_queue);

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
			if (vkQueueWaitIdle(vk_queue.first) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to wait queue!");
			std::pair<render_shared_resources*, uint32_t> rsr = rsr_pool.lock_and_grab();
			rsr.first->wait();
			rsr.first->reset();
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
		if (vkQueueWaitIdle(blender_vk_queue.first) != VK_SUCCESS)
						throw utilities::harpy_little_error("failed to wait present queue!");
		std::pair<render_shared_resources*, uint32_t> blender_rsr = rsr_pool.lock_and_grab();
		blender_rsr.first->wait();
		blender_rsr.first->reset();
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
		for (uint32_t i = 0; i < rendered_rsrs.size(); ++i) {
			rsr_pool.acquire(rendered_rsrs[i].second);
		}
		rsr_pool.acquire(blender_rsr.second);


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


	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = render_task;
	renderPassBeginInfo.framebuffer = rsr.first->fb;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };

	if (vkBeginCommandBuffer(rsr.first->cmd, &beginInfo) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to begin command buffer!");

	if (vkCmdBeginRenderPass(rsr.first->cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to begin render pass!");

	for object_mappings:
		vkCmdBindDescriptorSets(rsr.first->cmd);
		vkCmdPushConstants();
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, &offset);
		vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindPipeline(rsr.first->cmd);
		vkCmdSetViewport(*connected_buffer, 0, 1, &dynamic_viewport);
		vkCmdSetScissor(*connected_buffer, 0, 1, &dynamic_scissors);

		vkCmdDrawIndexed(rsr.first->cmd);

	if (vkCmdEndRenderPass(rsr.first->cmd) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to begin command buffer!");


	if (vkEndCommandBuffer(rsr.first->cmd) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to end command buffer!");

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


		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = blender_render_pass;
		renderPassBeginInfo.framebuffer = swapchain_fb;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };

		if (vkBeginCommandBuffer(rsr.first->cmd, &beginInfo) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin command buffer!");

		if (vkCmdBeginRenderPass(rsr.first->cmd, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin render pass!");


		if (blender_set && vkFreeDescriptorSets(spinal_cord->device, blender_desc_pool, 1, &blender_set) != VK_SUCCESS)
					throw utilities::harpy_little_error("failed to free descriptor set!");

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = blender_desc_pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &blender_descriptor_set_layout;

		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo, &blender_set) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");

		std::vector<VkDescriptorImageInfo> color_buffer(rendered_rsrs->size());
		std::vector<VkDescriptorImageInfo> depth_and_stencil_buffer(rendered_rsrs->size());

		for (uint32_t i = 0; i < rendered_rsrs->size(); i++) {
			color_buffer[i].imageView = (*rendered_rsrs)[i].first->color_image_view;
			color_buffer[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			color_buffer[i].sampler = blender_sampler;

			depth_and_stencil_buffer[i].imageView = (*rendered_rsrs)[i].first->depth_and_stencil_image_view;
			depth_and_stencil_buffer[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			depth_and_stencil_buffer[i].sampler = blender_sampler;
		}


		VkWriteDescriptorSet color_buffer_write_descriptor_set = {};
		color_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		color_buffer_write_descriptor_set.dstSet = blender_set;
		color_buffer_write_descriptor_set.dstBinding = 0;
		color_buffer_write_descriptor_set.dstArrayElement = 0;
		color_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		color_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		color_buffer_write_descriptor_set.pBufferInfo = color_buffer.data();


		VkWriteDescriptorSet depth_and_stencil_buffer_write_descriptor_set = {};
		depth_and_stencil_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depth_and_stencil_buffer_write_descriptor_set.dstSet = blender_set;
		depth_and_stencil_buffer_write_descriptor_set.dstBinding = 1;
		depth_and_stencil_buffer_write_descriptor_set.dstArrayElement = 0;
		depth_and_stencil_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		depth_and_stencil_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		depth_and_stencil_buffer_write_descriptor_set.pBufferInfo = color_buffer.data();

		std::vector<VkWriteDescriptorSet> writeDescriptorSets = {color_buffer_write_descriptor_set, depth_and_stencil_buffer_write_descriptor_set};

		if (vkUpdateDescriptorSets(spinal_cord->device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to update descriptor set!");


		if (vkCmdBindDescriptorSets(rsr.first->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, blender_pipeline_layout, 0, 0, &blender_set, 0, nullptr) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");

		blender_push_constants.layers_cnt = rendered_rsrs->size();

		if (vkCmdPushConstants(rsr.first->cmd, blender_pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(blender_push_constants), &blender_push_constants) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to bind push constants!");
			//vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, &offset);
		//vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);


		if (vkCmdBindPipeline(rsr.first->cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, blender_pipeline) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to bind pipeline!");

		if (vkCmdDraw(rsr.first->cmd,0,0,0,0) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to draw!");

		if (vkCmdEndRenderPass(rsr.first->cmd) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin command buffer!");


		if (vkEndCommandBuffer(rsr.first->cmd) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to end command buffer!");



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

	VkResult result = vkQueuePresentKHR(present_queue.first, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || connected_window_layout.get()->get_resize()) {
		connected_window_layout.get()->get_resize() = false;
		swapchain.reinit();
	} else if (result != VK_SUCCESS) {
		throw utilities::harpy_little_error("failed to present swap chain image!");
	}
}
