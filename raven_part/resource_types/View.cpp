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
	}

	void View::r_init(renderer_context* r_context){

		shaders::shader_module compute{r_context->spinal_cord->device};

		compute.init(SHADER_PATH_CAMERA_VERTEX);

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
		pipelineLayoutInfo.pSetLayouts = &r_context->vert_buf.vert_desc_layout;
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
			std::pair<VkBuffer, VkDeviceSize> vert_src,
			uint32_t region_i ,//buffer,offset
			Shape* shape,
			struct transform camera,
			struct transform object)
	{

		VkDeviceSize bufferSize = sizeof(Vertex) * (shape->vert_size);
		uint32_t offset = r_context->vert_buf.regions[region_i].offset;

		VkBufferCopy reg{};
		reg.srcOffset = vert_src.second;
		reg.dstOffset = r_context->vert_buf.regions[region_i].offset;
		reg.size = bufferSize;

		vkCmdCopyBuffer(cmd, vert_src.first, r_context->vert_buf.vert_tmp, 1, &reg);

		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT; // Предполагаемый доступ из первого шейдера
		bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;  // Предполагаемый доступ из второго шейдера
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = r_context->vert_buf.vert_tmp;  // Ваш буфер, данные которого используются во втором шейдере
		bufferBarrier.offset = offset;
		bufferBarrier.size = bufferSize;

		vkCmdPipelineBarrier(cmd,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);


		//VkDescriptorBufferInfo in_buffer;
		 //IDK
//		in_buffer.buffer = shape->vertexBuffer.first;
//		in_buffer.offset = 0;
//		in_buffer.range = bufferSize;//shape->vert_size; //IDK



//		VkWriteDescriptorSet in_desc_set_wr = {};
//		in_desc_set_wr.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//		in_desc_set_wr.dstSet = desc_set;
//		in_desc_set_wr.dstBinding = 0;
//		in_desc_set_wr.dstArrayElement = 0;
//		in_desc_set_wr.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
//		in_desc_set_wr.descriptorCount = 1;
//		in_desc_set_wr.pBufferInfo = &in_buffer;


		VkDescriptorSet sets3[] = {r_context->vert_buf.vert_desc};


		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, cameraPipelineLayout, 0, 1, sets3, 1, &offset);
		//
		CameraPushConstants cameraPushConstants;

		cameraPushConstants.transform = glm::scale(
				glm::mat4(1.0f),
				glm::vec3(1.0f/view_field.x, 1.0f/view_field.y, 1.0f)
				) *
						(camera.pos_mat4_reversed() * object.pos_mat4());

		switch (cameraType) {
			case CameraType::PERSPECTIVE:

				cameraPushConstants.transform = cameraPushConstants.transform * glm::mat4(
					glm::vec4(focus / aspectRatio, 0.0f , 0.0f, 0.0f),
					glm::vec4(0.0f, focus, 0.0f, 0.0f),
					glm::vec4(0.0f, 0.0f, -1.0f, -1.0f),
					glm::vec4(0.0f, 0.0f, -2.0f * focus, 0.0f)
				);
				break;
			case CameraType::ORTHOGRAPHIC:
				break;
			case CameraType::ISOMETRIC:

				cameraPushConstants.transform = cameraPushConstants.transform * glm::mat4(
			        glm::vec4(1.0f / glm::sqrt(2.0f), -1.0f / glm::sqrt(2.0f), 0.0f, 0.0f),
			        glm::vec4(1.0f / glm::sqrt(2.0f), 1.0f / glm::sqrt(2.0f), 0.0f, 0.0f),
			        glm::vec4(0.0f, 0.0f, glm::sqrt(2.0f), 0.0f),
			        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
			    );

				break;
			default:
				throw utilities::harpy_little_error("fake camera type");
		}





		vkCmdPushConstants(cmd,this->cameraPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(CameraPushConstants), &cameraPushConstants);
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->cameraGraphicsPipeline);

		vkCmdDispatch(cmd,shape->vert_size,1,1);



		// Барьер для синхронизации данных между первым и вторым vkCmdDispatch
		bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT; // Предполагаемый доступ из первого шейдера
		bufferBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;  // Предполагаемый доступ из второго шейдера
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = r_context->vert_buf.vert_tmp;  // Ваш буфер, данные которого используются во втором шейдере
		bufferBarrier.offset = offset;
		bufferBarrier.size = bufferSize;

		vkCmdPipelineBarrier(cmd,
		    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		    VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		    0,
		    0, nullptr,
		    1, &bufferBarrier,
		    0, nullptr);

	}

	void View::view_perform(VkCommandBuffer cmd){
		vkCmdSetViewport(cmd, 0, 1, &viewport);
		vkCmdSetScissor(cmd, 0, 1, &scissor);
	}

}


