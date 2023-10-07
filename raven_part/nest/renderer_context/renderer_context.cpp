/*
 * renderer_context.cpp
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#include <raven_part.h>

#include <windowing/base_window_layout.h>
#include <shaders/shader_module.h>
#include <ECS/ECS.h>
#include <utilities/utilities.h>
#include "renderer_context.h"

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
}

renderer_context::~renderer_context(){
	if (render_pass)
	vkDestroyRenderPass(this->spinal_cord->device, this->render_pass, nullptr);
}

void renderer_context::init_render_pass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = VK_FORMAT_B8G8R8A8_UNORM;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;


    VkAttachmentDescription depth_and_spencil_attachment{};

    depth_and_spencil_attachment.format = VK_FORMAT_D32_SFLOAT_S8_UINT;
    depth_and_spencil_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

    depth_and_spencil_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_and_spencil_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    depth_and_spencil_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_and_spencil_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    depth_and_spencil_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_and_spencil_attachment.finalLayout = VK_IMAGE_LAYOUT_GENERAL;


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

    VkSubpassDependency dependency1{};
    dependency1.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency1.dstSubpass = 0;
    dependency1.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency1.srcAccessMask = 0;
    dependency1.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency1.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkSubpassDependency dependency2{};
    dependency2.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency2.dstSubpass = 0;
	dependency2.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency2.srcAccessMask = 0;
	dependency2.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency2.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkSubpassDependency deps[] = {dependency1, dependency2};
    render_pass_create_info.dependencyCount = 1;
    render_pass_create_info.pDependencies = deps;

    if(vkCreateRenderPass(this->spinal_cord->device, &render_pass_create_info, nullptr, &this->render_pass) != VK_SUCCESS)
        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Render pass hasn't been properly initialised");
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
			VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT_EXT;

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

		shaders::shader_module fragment{spinal_cord->device};
		fragment.init(SHADER_PATH_BLENDER_FRAGMENT);

		VkPipelineShaderStageCreateInfo create_compute_info{};
		create_compute_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_compute_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		create_compute_info.module = fragment.module;
		create_compute_info.pName = "main";


		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.stage = create_compute_info;
		pipelineInfo.layout = blender_pipeline_layout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateComputePipelines(this->spinal_cord->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &blender_pipeline) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create graphics pipeline!");
		}

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
}




void renderer_context::init_swapchain(){
	swapchain.init();
	//connected_window_layout.init_all(instance);
}


void renderer_context::init_vert_tmp_buf(){
	VkDeviceSize bufferSize1 = utilities::align_to((VkDeviceSize)sizeof(Vertex) * (VkDeviceSize)tmp_storage->get_vert_max(), spinal_cord->deviceProperties.limits.minStorageBufferOffsetAlignment) * effective_rsr_cnt;
	if (bufferSize1 < bufferSize){
		if (RENDERER_MEMORY_OPTI_POLICY == RENDERER_MEMORY_OPTI_POLICY_PASSIVE) return;
	}
	bufferSize = bufferSize1;
	std::cout<<"bufferSize"<<std::endl;
	std::cout<<sizeof(Vertex)<<std::endl;

	VkDescriptorPoolSize pool_size{};
	pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_size.descriptorCount = effective_rsr_cnt*2;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &pool_size;
	poolInfo.maxSets = static_cast<uint32_t>(effective_rsr_cnt);
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

	if (vkCreateDescriptorPool(spinal_cord->device, &poolInfo, nullptr, &vert_desc_pool) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
				"failed to create descriptor pool!");
	}
	std::cout<<"init_vert_tmp_buf() vkCreateDescriptorPool"<<std::endl;
   VkBuffer stagingBuffer;
   VkDeviceMemory stagingBufferMemory;
   VkBufferCreateInfo bufferInfo{};
   bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
   bufferInfo.size = bufferSize;
   bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
   bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

   if (vkCreateBuffer(spinal_cord->device, &bufferInfo, nullptr, &vert_tmp) != VK_SUCCESS) {
	   throw std::runtime_error("failed to create buffer!");
   }
   std::cout<<"init_vert_tmp_buf() vkCreateBuffer"<<std::endl;
   VkMemoryRequirements memRequirements;
   vkGetBufferMemoryRequirements(spinal_cord->device, vert_tmp, &memRequirements);

   VkMemoryAllocateInfo allocInfo{};
   allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   allocInfo.allocationSize = memRequirements.size;
   allocInfo.memoryTypeIndex = harpy::utilities::find_memory_types(spinal_cord->ph_device,memRequirements.memoryTypeBits, (uint32_t)0);

   if (vkAllocateMemory(spinal_cord->device, &allocInfo, nullptr, &vert_tmp_mem) != VK_SUCCESS) {
	   throw std::runtime_error("failed to allocate buffer memory!");
   }
   std::cout<<"init_vert_tmp_buf() vkAllocateMemory"<<std::endl;
   std::cout<<"init_vert_tmp_buf() "<<vert_tmp_mem<<std::endl;
   vkBindBufferMemory(spinal_cord->device, vert_tmp, vert_tmp_mem, 0);
   std::cout<<"init_vert_tmp_buf() vkBindBufferMemory"<<std::endl;
}



void renderer_context::init_rsr_pool(){
	rsr_pool.init(effective_rsr_cnt);
}

void renderer_context::init_renderer_resource_storage(){

}

void renderer_context::init_renderer_object_mapper(){

}
void render_task_fake(
		renderer_context* ctx, std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<VkBuffer, uint32_t> vert_buf,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::pair<harpy::human_part::ECS::Transform*, uint32_t>* camera)
{
	ctx->render_task(rsr,vert_buf,vk_queue,camera);
}
//primitive
void renderer_context::render_loop(std::shared_ptr<harpy::raven_part::scene_source> source, std::atomic_flag* cond){


	tmp_storage = &source->storage;
	tmp_mapper = &source->mapper;

	std::cout<<"renderloop"<<std::endl;
	size_t frame = 0;
	uint32_t image_index{};
	uint32_t thread_cnt = std::min((uint32_t)std::thread::hardware_concurrency(), effective_rsr_cnt - 1);
	std::cout<<"renderloop1"<<std::endl;

	source->lock.lock();
	init_vert_tmp_buf();
	VkDeviceSize size_aligned = utilities::align_to((VkDeviceSize)sizeof(Vertex) * (VkDeviceSize)tmp_storage->get_vert_max(), spinal_cord->deviceProperties.limits.minStorageBufferOffsetAlignment);//storage.get_vert_max();
	source->lock.unlock();

	std::vector<std::pair<render_shared_resources*, uint32_t>> rendered_rsrs{};
	std::cout<<"renderloop2"<<std::endl;
	VkQueueFlags queue_flags = 0;
	if(USE_SHARED_RENDER_QUEUE){
		queue_flags = queue_flags | VK_QUEUE_GRAPHICS_BIT;
	}

	if(USE_SHARED_BLENDER_QUEUE){
		queue_flags = queue_flags | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;
	}

	std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> present_queue = spinal_cord->get_queue_supervisor().grab_presentation_queue(queue_flags, connected_window_layout->surface);
	std::vector<std::thread> threads{};
	std::cout<<"renderloop3"<<std::endl;


	while(cond->test_and_set(std::memory_order_acquire)){
		//should i wait present queue? in vulkan-guide there is no vkQueueWaitIdle(present_queue);

		rendered_rsrs.clear();
		threads.clear();
		//SPINLOCK
		std::cout<<"render_loop SPINLOCK"<<std::endl;
		while (source->consumed.test_and_set(std::memory_order_acquire)){}
		std::cout<<"render_loop FINISH"<<std::endl;
		source->lock.lock();
		std::cout<<"render_loop source->lock.lock()"<<std::endl;
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities = source->entities;
		std::cout<<"render_loop SOURCE"<<std::endl;
		std::cout<<"render_loop curr_camera: "<<source->curr_camera<<std::endl;
		std::cout<<"render_loop cameras size: "<<source->cameras.size()<<std::endl;
		human_part::ECS::Entity* src_camera = source->cameras[source->curr_camera];
		size_t ptr_size = sizeof(human_part::ECS::Entity*);
		source->consumed.test_and_set();

		std::cout<<"render_loop SOURCE1"<<std::endl;
		std::cout<<"render_loop SOURCE2"<<std::endl;

		std::cout<<thread_cnt<<std::endl;
		if (thread_cnt == 0) continue;
		std::cout<<"render_loop thread_cnt!=0"<<std::endl;
		std::cout<<"render_loop src_camera:"<<src_camera<<std::endl;
		harpy::human_part::ECS::Camera* camera_comp = dynamic_cast<harpy::human_part::ECS::Camera*>(src_camera->get_components_by_name(harpy::human_part::ECS::Camera::name)[0]);
		std::cout<<"render_loop camera_comp:"<<camera_comp<<std::endl;
		harpy::human_part::ECS::Transform* transform_comp = dynamic_cast<harpy::human_part::ECS::Transform*>(src_camera->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);
		std::cout<<"render_loop transform_comp:"<<transform_comp<<std::endl;
		std::pair<harpy::human_part::ECS::Transform*, uint32_t> camera = std::make_pair(transform_comp,camera_comp->view_id);

		std::cout<<"render_loop CAMERA"<<std::endl;

		std::cout<<"(int32_t)(entities->size())"<<std::endl;
		std::cout<<(int32_t)(entities->size())<<std::endl;
		uint32_t tgt_per_task = std::ceil((float)source->entities->size() / (float)thread_cnt);
		std::cout<<"tgt_per_task"<<std::endl;
		std::cout<<tgt_per_task<<std::endl;

		std::cout<<"render_loop TASKS PLANING"<<std::endl;

		tmp_mapper->lock.lock();

		renderer_context * context_ptr = this;
		//TODO UNEFFECTVE
		for (uint32_t i = 0; i < thread_cnt; ++i) {
			int32_t cnt = std::max(std::min((int32_t)tgt_per_task,  (int32_t)(entities->size()) - (int32_t)(i * tgt_per_task)),(int32_t)0);
			std::cout<<"cnt: "<<cnt<<std::endl;
			if(!cnt) continue;

			std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue;
			if(USE_SHARED_RENDER_QUEUE){
				vk_queue = present_queue;
			}else {
				vk_queue = spinal_cord->queue_supervisor.lock_grab(VK_QUEUE_GRAPHICS_BIT);
			}
			std::cout<<"got queue "<<vk_queue.second<<std::endl;
			std::cout<<vk_queue.first.first<<std::endl;
			if (vkQueueWaitIdle(vk_queue.first.first) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to wait queue!");
			std::cout<<"probe 2"<<std::endl;
			std::pair<render_shared_resources*, uint32_t> rsr = rsr_pool.lock_grab();
			std::cout<<"probe 2.1"<<std::endl;
			rsr.first->wait();
			std::cout<<"probe 2.2"<<std::endl;
			rsr.first->reset();
			std::cout<<"probe 2.5"<<std::endl;
			rsr.first->queue.resize(cnt);
			std::cout<<"probe 3"<<std::endl;
			for (int k = 0; k < cnt; ++k) {
				harpy::human_part::ECS::Entity* e = ((*entities)[i * tgt_per_task + k]);
				std::cout<<"probe 3.1"<<std::endl;
				std::cout<<i * tgt_per_task + k<<std::endl;
				std::cout<<e<<std::endl;
				auto components1 = e->get_components_by_name(harpy::human_part::ECS::Renderer::name);
				std::cout<<"probe 3.1.1"<<std::endl;
				if (components1.size() == 0) continue;
				std::cout<<"probe 3.1.2"<<std::endl;
				harpy::human_part::ECS::Renderer* renderer = dynamic_cast<harpy::human_part::ECS::Renderer*>(components1[0]);
				std::cout<<"probe 3.2"<<std::endl;
				auto components = e->get_components_by_name(harpy::human_part::ECS::Transform::name);
				std::cout<<"probe 3.2.1"<<std::endl;
				if (components.size() == 0) throw utilities::harpy_little_error("there are no transform component inside render task");
				std::cout<<"probe 3.2.2"<<std::endl;
				harpy::human_part::ECS::Transform* transform = dynamic_cast<harpy::human_part::ECS::Transform*>(components[0]);
				std::cout<<"probe 3.3"<<std::endl;
				rsr.first->queue[k].second = tmp_mapper->demap(renderer->mappings);
				std::cout<<"probe 3.4"<<std::endl;
				rsr.first->queue[k].first = transform;
			}
			std::cout<<"probe 4"<<std::endl;
			std::pair<VkBuffer, uint32_t> vert_buf = std::make_pair(vert_tmp, i * size_aligned);
			std::cout<<vert_tmp<<std::endl;
			threads.push_back(
					std::thread(
							render_task_fake,
							context_ptr,
							rsr,
							vert_buf,
							vk_queue,
							&camera)
						);
			rendered_rsrs.push_back(rsr);
		}
		std::cout<<"blender queue waiting"<<std::endl;
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> blender_vk_queue;
		if(USE_SHARED_BLENDER_QUEUE){
			blender_vk_queue = present_queue;
			//TODO
			if (vkQueueWaitIdle(blender_vk_queue.first.first) != VK_SUCCESS)
										throw utilities::harpy_little_error("failed to wait blender queue!");
		}else {
			blender_vk_queue = spinal_cord->queue_supervisor.lock_grab(VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
			//TODO
			if (vkQueueWaitIdle(blender_vk_queue.first.first) != VK_SUCCESS)
							throw utilities::harpy_little_error("failed to wait blender queue!");
		}

		std::pair<render_shared_resources*, uint32_t> blender_rsr;
		std::cout<<"grab blender_rsr"<<std::endl;
		blender_rsr = rsr_pool.lock_grab();
		blender_rsr.first->wait();
		blender_rsr.first->reset();
		tmp_mapper->lock.unlock();

		VkResult result = vkAcquireNextImageKHR(spinal_cord->device,
					swapchain, UINT32_MAX,
					swapchain.image_sems[frame],
					VK_NULL_HANDLE,
					&image_index);

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
		std::cout<<"WAITING"<<std::endl;
		for (int i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}
		std::cout<<"waited"<<std::endl;
		source->lock.unlock();
		std::cout<<"unlocked"<<std::endl;
		if(!USE_SHARED_RENDER_QUEUE){
			blending(blender_rsr, blender_vk_queue, &rendered_rsrs, swapchain.images[image_index], swapchain.image_views[image_index], swapchain.image_sems[frame]);
			present(present_queue,blender_rsr,image_index);
		}else{
			copy_render_res(blender_rsr, blender_vk_queue, rendered_rsrs[0], swapchain.images[image_index], swapchain.image_views[image_index], swapchain.image_sems[frame]);
			present(present_queue,blender_rsr,image_index);
		}


		for (int i = 0; i < rendered_rsrs.size(); ++i) {
			rsr_pool.lock_free(rendered_rsrs[i].second);
		}
		std::cout<<"rsr_pool.lock_free"<<std::endl;
		rsr_pool.lock_free(blender_rsr.second);

		if(!USE_SHARED_BLENDER_QUEUE){
			spinal_cord->queue_supervisor.lock_free(blender_vk_queue.second);
			std::cout<<"queue_supervisor.lock_free"<<std::endl;
		}


		frame = (frame + 1) % swapchain.image_sems.size();

	}
	vkDeviceWaitIdle(spinal_cord->device);

	spinal_cord->queue_supervisor.lock_free(present_queue.second);
}

void renderer_context::render_task(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<VkBuffer, uint32_t> vert_buf,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::pair<harpy::human_part::ECS::Transform*, uint32_t>* camera)
{
	std::cout<<"render_task()"<<std::endl;
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


	std::cout<<"views.size: "<<tmp_storage->views.size()<<std::endl;
	std::cout<<"(*camera).second: "<<(*camera).second<<std::endl;
	resource_types::View view = tmp_storage->views[(*camera).second];
	std::cout<<"(*camera).second"<<std::endl;
	std::cout<<(*camera).second<<std::endl;




	for (uint32_t i = 0; i < rsr.first->queue.size(); i++){
		std::pair<harpy::human_part::ECS::Transform*, std::vector<renderer_mappings>> entity = rsr.first->queue[i];
		for (int i_prim = 0; i_prim < entity.second.size(); ++i_prim){

			vert_desc_pool_lock.lock();
			rsr.first->reinit_vertex_tmp(vert_desc_pool, view.desc_set_layout);
			vert_desc_pool_lock.unlock();

			if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to begin command buffer!");

			resource_types::Shape shape = tmp_storage->shapes[entity.second[i_prim].shape_id];
			resource_types::Material material = tmp_storage->materials[entity.second[i_prim].material_id];
			//std::cout<<"probe render_task 4"<<std::endl;
	//=====================
	//    SHAPE
	//=====================
			//shape.perform(vk_queue.first.second);
	//=====================
	//    VIEW & CAMERA TRANSPOSE
	//=====================
			view.camera_perform(
					vk_queue.first.second,
					rsr.first->vert_desc,
					vert_buf,
					&shape,
					(*camera).first,
					entity.first
					);
			//std::cout<<"probe render_task 5"<<std::endl;
			vkCmdBeginRenderPass(vk_queue.first.second, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

			view.view_perform(vk_queue.first.second);

			VkDeviceSize offsets[] = {vert_buf.second};
			vkCmdBindVertexBuffers(vk_queue.first.second, 0, 1, &vert_buf.first, offsets);
			vkCmdBindIndexBuffer(vk_queue.first.second, shape.indexBuffer.first, 0, shape.indexType);
	//=====================
	//    MATERIAL
	//=====================
			material.perform(vk_queue.first.second, &shape);
			vkCmdEndRenderPass(vk_queue.first.second);
	//=====================
	//    EFFECT
	//=====================

	//	   vkCmdBindPipeline(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipeline);
	//
	//	   vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, 1, &pipeline.desc_set, 0, nullptr);

			if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to end command buffer!");


		}
	}

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

	if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, rsr.first->fence1) != VK_SUCCESS) {
		throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
	}

	rsr.first->color_image_layout = VK_IMAGE_LAYOUT_GENERAL;
	rsr.first->depth_and_stencil_image_layout = VK_IMAGE_LAYOUT_GENERAL;
	if(!USE_SHARED_RENDER_QUEUE){
		spinal_cord->queue_supervisor.lock_free(vk_queue.second);
	}
	std::cout<<"render_task() exit"<<std::endl;
}


void renderer_context::blending(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::vector<std::pair<render_shared_resources*, uint32_t>>* rendered_rsrs,
		VkImage swapchain_image,
		VkImageView swapchain_image_view,
		VkSemaphore image_sem)
{

	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to reset cmd RSR!");

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to begin command buffer!");

		if (rsr.first->blender_set1){
			VkDescriptorSet sets[] = {rsr.first->blender_set1,rsr.first->blender_set2,rsr.first->blender_set_out};
			if (rsr.first->blender_set1 && vkFreeDescriptorSets(spinal_cord->device, rsr.first->blender_desc_pool, 3, sets) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to free descriptor set!");
		}

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = rsr.first->blender_desc_pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &blender_descriptor_set_layout_out;

		if (vkAllocateDescriptorSets(this->spinal_cord->device, &allocInfo, &rsr.first->blender_set_out) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to allocate descriptor set!");


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

		vkUpdateDescriptorSets(spinal_cord->device, 3, writeDescriptorSets, 0, nullptr);

		VkDescriptorSet sets3[] = {rsr.first->blender_set1, rsr.first->blender_set2, rsr.first->blender_set_out};

		vkCmdBindDescriptorSets(vk_queue.first.second, VK_PIPELINE_BIND_POINT_COMPUTE, blender_pipeline_layout, 0, 3, sets3, 0, nullptr);

		blender_push_constants.layers_cnt = rendered_rsrs->size();
		vkCmdPushConstants(vk_queue.first.second, blender_pipeline_layout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(blender_push_constants), &blender_push_constants);


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

		if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
			throw utilities::harpy_little_error("failed to end command buffer!");

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

		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vk_queue.first.second;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &rsr.first->sem;

		if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, rsr.first->fence1) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
		}

		for (int i = 0; i < rendered_rsrs->size(); ++i) {
			(*rendered_rsrs)[i].first->blender_fence = rsr.first->fence1;
		}

		rsr.first->blender_fence = nullptr;
		rsr.first->color_image_layout = VK_IMAGE_LAYOUT_GENERAL;
}




void renderer_context::copy_render_res(
		std::pair<render_shared_resources*, uint32_t> rsr,
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
		std::pair<render_shared_resources*, uint32_t> rendered_rsr,
		VkImage swapchain_image,
		VkImageView swapchain_image_view,
		VkSemaphore image_sem)
{
	if (vkResetCommandBuffer(vk_queue.first.second, 0) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to reset cmd RSR!");

			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(vk_queue.first.second, &beginInfo) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to begin command buffer!");


			VkImageMemoryBarrier imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageBarrier.subresourceRange.baseMipLevel = 0;
			imageBarrier.subresourceRange.levelCount = 1;
			imageBarrier.subresourceRange.baseArrayLayer = 0;
			imageBarrier.subresourceRange.layerCount = 1;

			VkImageCopy imageCopy{};
			imageCopy.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopy.srcSubresource.layerCount = 1;
			imageCopy.srcOffset = { 0, 0, 0 };
			imageCopy.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageCopy.dstSubresource.layerCount = 1;
			imageCopy.dstOffset = { 0, 0, 0 };
			imageCopy.extent = { swapchain.extent.width, swapchain.extent.height, 1 };

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

			vkCmdCopyImage(
				vk_queue.first.second,
			    rendered_rsr.first->color_image,
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

			if (vkEndCommandBuffer(vk_queue.first.second) != VK_SUCCESS)
				throw utilities::harpy_little_error("failed to end command buffer!");

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			std::vector<VkSemaphore> waitSemaphores;
			std::vector<VkPipelineStageFlags> waitStages;
			waitSemaphores.reserve(2);
			waitStages.reserve(2);

			waitSemaphores.push_back(rendered_rsr.first->sem);
			waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);

			waitSemaphores.push_back(image_sem);
			waitStages.push_back(VK_PIPELINE_STAGE_TRANSFER_BIT);

			submitInfo.waitSemaphoreCount = waitSemaphores.size();
			submitInfo.pWaitSemaphores = waitSemaphores.data();
			submitInfo.pWaitDstStageMask = waitStages.data();
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &vk_queue.first.second;
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &rsr.first->sem;

			if (vkQueueSubmit(vk_queue.first.first, 1, &submitInfo, rsr.first->fence1) != VK_SUCCESS) {
				throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
			}

			rendered_rsr.first->blender_fence = rsr.first->fence1;

			rsr.first->blender_fence = nullptr;
			rsr.first->color_image_layout = VK_IMAGE_LAYOUT_GENERAL;

}



void renderer_context::present(
		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> present_queue,
		std::pair<render_shared_resources*, uint32_t> rsr,
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
	connected_window_layout->init(this);
	init_swapchain();
	init_render_pass();
	init_rsr_pool();
	init_renderer_resource_storage();
	init_renderer_object_mapper();
	init_blender();
	swapchain.init_image_views();

}





