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

	delete &rsr_pool;
	if (render_pass)
	vkDestroyRenderPass(this->spinal_cord->device, this->render_pass, nullptr);
	if (pipeline_layout)
	vkDestroyPipelineLayout(this->spinal_cord->device, this->pipeline_layout, nullptr);
	//if (descriptor_set_layout)
	//vkDestroyDescriptorSetLayout(this->spinal_cord->device, this->descriptor_set_layout, nullptr);
	delete &swapchain;

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
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


    VkAttachmentDescription depth_and_spencil_attachment{};
    depth_and_spencil_attachment.format = VK_FORMAT_D24_UNORM_S8_UINT;
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
    depth_and_spencil_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;//&this->descriptor_set_layout;

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

	    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
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

		std::cout<<"init vkCreateRenderPass"<<std::endl;
		VkDescriptorSetLayoutBinding colorLayoutBinding{};
		colorLayoutBinding.binding = 0;
		//colorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		colorLayoutBinding.descriptorCount = effective_rsr_cnt;
		colorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		colorLayoutBinding.pImmutableSamplers = nullptr;
		colorLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;



		VkDescriptorSetLayoutBinding depthLayoutBinding{};
		depthLayoutBinding.binding = 0;
		//depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		depthLayoutBinding.descriptorCount = effective_rsr_cnt;
		depthLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depthLayoutBinding.pImmutableSamplers = nullptr;
		depthLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;


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


		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo1, nullptr, &this->blender_descriptor_set_layout1) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor set layout!");
		}
		if (vkCreateDescriptorSetLayout(this->spinal_cord->device, &layoutInfo2, nullptr, &this->blender_descriptor_set_layout2) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor set layout!");
		}
		std::cout<<"init vkCreateDescriptorSetLayout"<<std::endl;
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(blender_push_constants);

		VkDescriptorSetLayout layouts[] = {blender_descriptor_set_layout1,blender_descriptor_set_layout2};

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 2;
		pipelineLayoutInfo.pSetLayouts = layouts;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(this->spinal_cord->device, &pipelineLayoutInfo, nullptr, &this->blender_pipeline_layout) != VK_SUCCESS) {
			 throw std::runtime_error("failed to create pipeline layout!");
		}
		std::cout<<"init vkCreatePipelineLayout"<<std::endl;





		VkViewport blender_dynamic_viewport;
		VkRect2D blender_dynamic_scissors;



		shaders::shader_module vertex{spinal_cord->device};
		shaders::shader_module fragment{spinal_cord->device};
		std::cout<<"init shaders 11"<<std::endl;
		vertex.init(SHADER_PATH_BLENDER_VERTEX_DUMMY);
		fragment.init(SHADER_PATH_BLENDER_FRAGMENT);
		std::cout<<"init shaders succ"<<std::endl;
		VkPipelineShaderStageCreateInfo create_vertex_info{};
		create_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_vertex_info.stage = VK_SHADER_STAGE_VERTEX_BIT;

		create_vertex_info.module = vertex;
		create_vertex_info.pName = "main";

		VkPipelineShaderStageCreateInfo create_fragment_info{};
		create_fragment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_fragment_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		create_fragment_info.module = fragment.module;
		create_fragment_info.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {create_vertex_info,create_fragment_info}; //create_vertex_info
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
		pipelineInfo.flags = 0;
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

void renderer_context::init_descriptor_pool()
	{

	VkDescriptorPoolSize pool_size{};
	pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_size.descriptorCount = static_cast<uint32_t>(1);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &pool_size;
	poolInfo.maxSets = static_cast<uint32_t>(2);

	if (vkCreateDescriptorPool(this->spinal_cord->device, &poolInfo, nullptr, &blender_desc_pool) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor pool!");
	}

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

//primitive
void renderer_context::render_loop(std::shared_ptr<harpy::raven_part::object_source> source, std::atomic_flag* cond){
	std::cout<<"probe0"<<std::endl;
	size_t frame = 0;
	uint32_t image_index{};
	uint32_t task_cnt = std::min((uint32_t)std::thread::hardware_concurrency(), effective_rsr_cnt);
	std::vector<std::pair<render_shared_resources*, uint32_t>> rendered_rsrs;
	std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> present_queue = spinal_cord->get_queue_supervisor().grab_presentation_queue((VkQueueFlags)0, connected_window_layout->surface);


	std::cout<<"probe1"<<std::endl;
	while(cond->test_and_set(std::memory_order_acquire)){
		//should i waip present queue? in vulkan-guide there is no vkQueueWaitIdle(present_queue);

		//SPINLOCK
		rendered_rsrs.empty();
		while (source->consumed.test_and_set(std::memory_order_acquire)){}
		std::cout<<"probe2"<<std::endl;
		source->lock.lock();
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities = source->entities;
		size_t ptr_size = sizeof(human_part::ECS::Entity*);
		source->consumed.test_and_set();
		source->lock.unlock();
		std::cout<<"probe3"<<std::endl;
		if (task_cnt == 0) continue;
		uint32_t tgt_per_task = std::ceil((float)source->entities->size() / (float)task_cnt);
		//if (tgt_per_task == 0) continue;
		std::cout<<"probe3.1"<<std::endl;

		std::cout<<"probe3.2"<<std::endl;
		spinal_cord->queue_supervisor.lock.lock();
		rsr_pool.lock.lock();
		mapper.lock.lock();

		renderer_context * context_ptr = this;
		std::cout<<"probe4"<<std::endl;

		for (uint32_t i = 0; i < task_cnt; ++i) {
			std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue = spinal_cord->queue_supervisor.grab(VK_QUEUE_GRAPHICS_BIT);
			if (vkQueueWaitIdle(vk_queue.first.first) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to wait queue!");
			std::cout<<"probe4.1"<<std::endl;
			std::pair<render_shared_resources*, uint32_t> rsr = rsr_pool.grab();
			std::cout<<"probe4.1.1"<<std::endl;
			rsr.first->wait();
			std::cout<<"probe4.1.2"<<std::endl;
			rsr.first->reset();
			std::cout<<"probe4.2"<<std::endl;
				//reset sems
			//copy to subqueues

			uint32_t cnt = std::min(i * tgt_per_task,  (uint32_t)(rsr.first->queue.size()) - i * tgt_per_task);
			rsr.first->queue.resize(cnt);
			for (int k = 0; k < cnt; ++k) {
				rsr.first->queue[k].first = (*entities)[i * tgt_per_task + k];
				std::vector<harpy::human_part::ECS::Component*> comps = ((*entities)[i * tgt_per_task + k])->get_components_by_name(harpy::human_part::ECS::Renderer::name);
				uint32_t mappings_id = dynamic_cast<harpy::human_part::ECS::Renderer*>(comps[0])->mapping_id;
				rsr.first->queue[k].second = mapper.mappings[mappings_id];
			}
			std::cout<<"probe4.3"<<std::endl;
			/*std::memcpy(
					entities->data() + ptr_size * i * tgt_per_task,
					rsr.first->queue.data(),
					ptr_size * cnt);*/
			std::async(std::launch::async, [rsr, vk_queue, context_ptr]() {
				context_ptr->render_task(rsr,vk_queue);
			    });
			std::cout<<"probe4.4"<<std::endl;
			rendered_rsrs.push_back(rsr);
		}
		std::cout<<"probe5"<<std::endl;
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> blender_vk_queue = spinal_cord->queue_supervisor.grab(VK_QUEUE_GRAPHICS_BIT);
		if (vkQueueWaitIdle(blender_vk_queue.first.first) != VK_SUCCESS)
						throw utilities::harpy_little_error("failed to wait present queue!");
		std::cout<<"probe5.1"<<std::endl;
		std::pair<render_shared_resources*, uint32_t> blender_rsr = rsr_pool.grab();
		blender_rsr.first->wait();
		blender_rsr.first->reset();
		mapper.lock.unlock();
		spinal_cord->queue_supervisor.lock.unlock();
		rsr_pool.lock.unlock();
		std::cout<<"probe6"<<std::endl;

		VkResult result = vkAcquireNextImageKHR(spinal_cord->device,
					swapchain, UINT32_MAX,
					swapchain.image_sems[frame], VK_NULL_HANDLE, &image_index);
		std::cout<<"probe7"<<std::endl;
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			swapchain.reinit();
			//rsr_pool.fb_resize();
			//for(auto& i : framebuffs)
			//	vkDestroyFramebuffer(vulkan_backend->get_vk_device(), i, nullptr);
			return;
		}
		std::cout<<"probe8"<<std::endl;
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw utilities::harpy_little_error("failed to acquire swap chain image!");
		}
		std::cout<<"probe9"<<std::endl;
		blending(blender_rsr, blender_vk_queue, &rendered_rsrs, swapchain.fbs[image_index], swapchain.image_sems[frame]);
		std::cout<<"probe10"<<std::endl;
		present(present_queue,blender_rsr,image_index);
		std::cout<<"probe11"<<std::endl;
		//for (uint32_t i = 0; i < rendered_rsrs.size(); ++i) {
		//	rsr_pool.acquire(rendered_rsrs[i].second);
		//}
		rsr_pool.lock_free(blender_rsr.second);


		frame = (frame + 1) % swapchain.image_sems.size();
		std::cout<<"probe12"<<std::endl;


	}
	vkDeviceWaitIdle(spinal_cord->device);
	rsr_pool.free(present_queue.second);
}

void renderer_context::render_task(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue
		)
{
	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to reset cmd RSR!");


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

	vkCmdBeginRenderPass(vk_queue.first.second, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

//	for object_mappings:
//		vkCmdBindDescriptorSets(vk_queue.first.second);
//		vkCmdPushConstants(vk_queue.first.second);
//		vkCmdBindVertexBuffers(vk_queue.first.second, 0, 1, &object.vertexBuffer, &offset);
//		vkCmdBindIndexBuffer(vk_queue.first.second, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//
//		vkCmdBindPipeline(vk_queue.first.second);
//		vkCmdSetViewport(vk_queue.first.second, 0, 1, &dynamic_viewport);
//		vkCmdSetScissor(vk_queue.first.second, 0, 1, &dynamic_scissors);
//
//		vkCmdDrawIndexed(vk_queue.first.second);

	vkCmdEndRenderPass(vk_queue.first.second);


	if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
		throw utilities::harpy_little_error("failed to end command buffer!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//uint64_t signal_value = 1;
//	VkTimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo = {};
//	timelineSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
//	timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = 0;
//	timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = VK_NULL_HANDLE;
//	timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = 1;
//	timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = rsr.first->curr_sem_val + 1;

	//VkSemaphore waitSemaphores[] = {image_sems[frame]};
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = VK_NULL_HANDLE;//waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &vk_queue.first.second;
	//submitInfo.pNext = &timelineSemaphoreSubmitInfo;


	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &rsr.first->sem;

	if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
	}

	rsr_pool.free(rsr.second);

}


void renderer_context::blending(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::vector<std::pair<render_shared_resources*, uint32_t>>* rendered_rsrs,
		VkFramebuffer swapchain_fb,
		VkSemaphore image_sem
		)
{
	std::cout<<"probe blending0"<<std::endl;
	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to reset cmd RSR!");
	std::cout<<"probe blending1"<<std::endl;
	VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;


		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = blender_render_pass;
		renderPassBeginInfo.framebuffer = swapchain_fb;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { swapchain.extent.width, swapchain.extent.height };
		renderPassBeginInfo.pClearValues = &clear_color;
		renderPassBeginInfo.clearValueCount = 1;

		if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin command buffer!");
		std::cout<<"probe blending2"<<std::endl;
		vkCmdBeginRenderPass(vk_queue.first.second, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		std::cout<<"probe blending3"<<std::endl;
		VkDescriptorSet sets[] = {blender_set1,blender_set2};
		if (blender_set1 && vkFreeDescriptorSets(spinal_cord->device, blender_desc_pool, 2, sets) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to free descriptor set!");
		std::cout<<"probe blending4"<<std::endl;
//		if (blender_set2 && vkFreeDescriptorSets(spinal_cord->device, blender_desc_pool, 1, &blender_set2) != VK_SUCCESS)
//			throw utilities::harpy_little_error("failed to free descriptor set!");
//		std::cout<<"probe blending5"<<std::endl;

		VkDescriptorSetLayout layouts[] = {blender_descriptor_set_layout1, blender_descriptor_set_layout2};
		VkDescriptorSetAllocateInfo allocInfo1{};
		allocInfo1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo1.descriptorPool = blender_desc_pool;
		allocInfo1.descriptorSetCount = 2;
		allocInfo1.pSetLayouts = layouts;

//		VkDescriptorSetAllocateInfo allocInfo2{};
//		allocInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		allocInfo2.descriptorPool = blender_desc_pool;
//		allocInfo2.descriptorSetCount = 1;
//		allocInfo2.pSetLayouts = &blender_descriptor_set_layout2;

		VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescriptorCountInfo{};
		uint32_t sizes[] = {rendered_rsrs->size(),rendered_rsrs->size()};
		variableDescriptorCountInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		variableDescriptorCountInfo.descriptorSetCount = 2;
		variableDescriptorCountInfo.pDescriptorCounts = sizes;

		allocInfo1.pNext = &variableDescriptorCountInfo;

		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo1, &blender_set1) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");
		std::cout<<"probe blending6"<<std::endl;
//		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo2, &blender_set2) != VK_SUCCESS)
//			throw utilities::harpy_little_error("failed to allocate descriptor set!");
//		std::cout<<"probe blending7"<<std::endl;

		std::vector<VkDescriptorImageInfo> color_buffer(rendered_rsrs->size());
		std::vector<VkDescriptorImageInfo> depth_buffer(rendered_rsrs->size());

		for (uint32_t i = 0; i < rendered_rsrs->size(); i++) {
			color_buffer[i].imageView = (*rendered_rsrs)[i].first->color_image_view;
			color_buffer[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			color_buffer[i].sampler = blender_sampler;

			depth_buffer[i].imageView = (*rendered_rsrs)[i].first->depth_image_view;
			depth_buffer[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
			depth_buffer[i].sampler = blender_sampler;
		}

		std::cout<<rendered_rsrs->size()<<std::endl;
		VkWriteDescriptorSet color_buffer_write_descriptor_set = {};
		color_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		color_buffer_write_descriptor_set.dstSet = blender_set1;
		color_buffer_write_descriptor_set.dstBinding = 0;
		color_buffer_write_descriptor_set.dstArrayElement = 0;
		color_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		color_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		color_buffer_write_descriptor_set.pImageInfo = color_buffer.data();


		VkWriteDescriptorSet depth_and_stencil_buffer_write_descriptor_set = {};
		depth_and_stencil_buffer_write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		depth_and_stencil_buffer_write_descriptor_set.dstSet = blender_set2;
		depth_and_stencil_buffer_write_descriptor_set.dstBinding = 0;
		depth_and_stencil_buffer_write_descriptor_set.dstArrayElement = 0;
		depth_and_stencil_buffer_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		depth_and_stencil_buffer_write_descriptor_set.descriptorCount = rendered_rsrs->size();
		depth_and_stencil_buffer_write_descriptor_set.pImageInfo = depth_buffer.data();

		VkWriteDescriptorSet writeDescriptorSets[] = {color_buffer_write_descriptor_set, depth_and_stencil_buffer_write_descriptor_set};
		std::cout<<"probe blending8"<<std::endl;
		vkUpdateDescriptorSets(spinal_cord->device, 2, writeDescriptorSets, 0, nullptr);
		VkDescriptorSet sets2[] = {blender_set1, blender_set2};
		std::cout<<"probe blending9"<<std::endl;
		vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, blender_pipeline_layout, 0, 2, sets2, 0, nullptr);
		std::cout<<"probe blending10"<<std::endl;
		blender_push_constants.layers_cnt = rendered_rsrs->size();
		std::cout<<"probe blending11"<<std::endl;
		vkCmdPushConstants(vk_queue.first.second, blender_pipeline_layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(blender_push_constants), &blender_push_constants);
			//vkCmdBindVertexBuffers(commandBuffer, 0, 1, &object.vertexBuffer, &offset);
		//vkCmdBindIndexBuffer(commandBuffer, object.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		std::cout<<"probe blending12"<<std::endl;
		vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, blender_pipeline);
		std::cout<<"probe blending13"<<std::endl;
		vkCmdDraw(vk_queue.first.second,0,0,0,0);
		std::cout<<"probe blending14"<<std::endl;
		vkCmdEndRenderPass(vk_queue.first.second);

		std::cout<<"probe blending15"<<std::endl;
		if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to end command buffer!");


		std::cout<<"probe blending16"<<std::endl;
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
		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			signalSemaphores.push_back((*rendered_rsrs)[i].first->sem2);
		}
		signalSemaphores.push_back(rsr.first->sem);
		signalSemaphores.push_back(rsr.first->sem2);



//		VkTimelineSemaphoreSubmitInfo timelineSemaphoreSubmitInfo = {};
//		timelineSemaphoreSubmitInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
//		timelineSemaphoreSubmitInfo.waitSemaphoreValueCount = waitSemaphores.size();
//		timelineSemaphoreSubmitInfo.pWaitSemaphoreValues = wait_values.data();
//		timelineSemaphoreSubmitInfo.signalSemaphoreValueCount = signalSemaphores.size();
//		timelineSemaphoreSubmitInfo.pSignalSemaphoreValues = signal_values.data();



		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vk_queue.first.second;
		//submitInfo.pNext = &timelineSemaphoreSubmitInfo;


		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();
		std::cout<<"probe blending17"<<std::endl;
		if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
		}



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
	init_descriptor_pool();
	std::cout<<"init_descriptor_pool succ"<<std::endl;
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



