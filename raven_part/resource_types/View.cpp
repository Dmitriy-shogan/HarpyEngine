/*
 * View.cpp
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#include "View.h";
#include <renderer_context/renderer_context.h>
#include <ECS/components/Transform.h>
#include <raven_part.h>


namespace harpy::raven_part::resource_types{

	void View::load(tinygltf::Model& model, load_package pack){
		this->r_context = pack.r_context_ptr;

		shaders::shader_module compute{r_context->spinal_cord->device};
		switch (cameraType) {
			case CameraType::PRESPECTIVE:
				compute.init(SHADER_PATH_CAMERA_PRESPECTIVE_VERTEX);
				break;
			case CameraType::ORTHOGRAPHIC:
				compute.init(SHADER_PATH_CAMERA_ORTHOGRAPHIC_VERTEX);
				break;
			default:
				throw utilities::harpy_little_error("fake camera type");
		}


		VkDescriptorSetLayoutBinding inLayoutBinding{};
		inLayoutBinding.binding = 0;
		inLayoutBinding.descriptorCount = 1;
		inLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		inLayoutBinding.pImmutableSamplers = nullptr;
		inLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding outLayoutBinding{};
		outLayoutBinding.binding = 1;
		outLayoutBinding.descriptorCount = 1;
		outLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		outLayoutBinding.pImmutableSamplers = nullptr;
		outLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutBinding bindings[] = {inLayoutBinding, outLayoutBinding};
		VkDescriptorSetLayoutCreateInfo layoutInfo3{};
		layoutInfo3.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo3.bindingCount = 2;
		layoutInfo3.pBindings = bindings;

		if (vkCreateDescriptorSetLayout(r_context->spinal_cord->device, &layoutInfo3, nullptr, &desc_set_layout) != VK_SUCCESS) {
				throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
					"failed to create descriptor set layout!");
			}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		vertShaderStageInfo.module = compute.module;
		vertShaderStageInfo.pName = "main";



		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(CameraPushConstants);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &desc_set_layout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(r_context->spinal_cord->device, &pipelineLayoutInfo, nullptr, &cameraPipelineLayout) != VK_SUCCESS) {
		   throw std::runtime_error("failed to create pipeline layout!");
		}

		VkPipelineShaderStageCreateInfo create_compute_info{};
		create_compute_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_compute_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		create_compute_info.module = compute.module;
		create_compute_info.pName = "main";


		VkComputePipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		pipelineInfo.flags = 0;
		pipelineInfo.stage = create_compute_info;
		pipelineInfo.layout = cameraPipelineLayout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateComputePipelines(r_context->spinal_cord->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &cameraGraphicsPipeline) != VK_SUCCESS) {
			throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create graphics pipeline!");
		}

	}

	void View::camera_perform(
			VkCommandBuffer cmd,
			VkDescriptorSet desc_set,
			std::pair<VkBuffer, VkDeviceSize> vert_tmp,//buffer,offset
			Shape* shape,
			human_part::ECS::Transform* camera,
			human_part::ECS::Transform* object)
	{
		//
		VkDeviceSize bufferSize = sizeof(Vertex) * shape->vert_size;

		VkDescriptorBufferInfo in_buffer;
		VkDescriptorBufferInfo out_buffer;

		in_buffer.buffer = shape->vertexBuffer.first;
		in_buffer.offset = 0;
		in_buffer.range = bufferSize;//shape->vert_size; //IDK

		out_buffer.buffer = vert_tmp.first;
		out_buffer.offset = vert_tmp.second;
		out_buffer.range = bufferSize;//shape->vert_size; //IDK

		VkWriteDescriptorSet in_desc_set_wr = {};
		in_desc_set_wr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		in_desc_set_wr.dstSet = desc_set;
		in_desc_set_wr.dstBinding = 0;
		in_desc_set_wr.dstArrayElement = 0;
		in_desc_set_wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		in_desc_set_wr.descriptorCount = 1;
		in_desc_set_wr.pBufferInfo = &in_buffer;


		VkWriteDescriptorSet out_desc_set_wr = {};
		out_desc_set_wr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		out_desc_set_wr.dstSet = desc_set;
		out_desc_set_wr.dstBinding = 1;
		out_desc_set_wr.dstArrayElement = 0;
		out_desc_set_wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		out_desc_set_wr.descriptorCount = 1;
		out_desc_set_wr.pBufferInfo = &out_buffer;

		VkWriteDescriptorSet writeDescriptorSets[] = {in_desc_set_wr,out_desc_set_wr};

		vkUpdateDescriptorSets(r_context->spinal_cord->device, 2, writeDescriptorSets, 0, nullptr);
		//
		VkDescriptorSet sets3[] = {desc_set};

		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, cameraPipelineLayout, 0, 1, sets3, 0, nullptr);
		//
		CameraPushConstants cameraPushConstants;


//		human_part::ECS::Transform * relative = object->relative_to(camera);
//		cameraPushConstants.transform = relative->rot_mat4() * (glm::mat4(1.0f) * (1.0f/relative->pos_vec4().length()));
		//object->pos_mat * (-camera->pos_mat)
		cameraPushConstants.transform = glm::scale(glm::mat4(1.0f),glm::vec3(1.0f/view_field.x, 1.0f/view_field.y, 1.0f)) * (camera->pos_mat4_reversed() * object->pos_mat4());
		cameraPushConstants.translate = glm::vec4(0.0f,0.0f,0.0f,0.0f); //relative->pos_vec4();
//
//
//
//
//

//
//
//
		vkCmdPushConstants(cmd,this->cameraPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(CameraPushConstants), &cameraPushConstants);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->cameraGraphicsPipeline);

		vkCmdDispatch(cmd,shape->vert_size,1,1);
	}

	void View::view_perform(VkCommandBuffer cmd){
		vkCmdSetViewport(cmd, 0, 1, &viewport);
		vkCmdSetScissor(cmd, 0, 1, &scissor);
	}

}


