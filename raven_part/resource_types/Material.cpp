/*
 * Material.cpp
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#include "Material.h"
#include <renderer_context/renderer_context.h>
#include <raven_part.h>

namespace harpy::raven_part::resource_types{


	VkDescriptorSetLayout createDescriptorSetLayout(std::shared_ptr<vulkan_spinal_cord> cord) {
		   VkDescriptorSetLayout descriptorSetLayout;
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

	       if (vkCreateDescriptorSetLayout(cord->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
	           throw std::runtime_error("failed to create descriptor set layout!");
	       }
	       return descriptorSetLayout;
	   }


	   std::pair<VkPipeline,VkPipelineLayout> createGraphicsPipeline(std::shared_ptr<vulkan_spinal_cord> cord,VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderpass) {
		   VkPipelineLayout pipelineLayout;
		   VkPipeline graphicsPipeline;

		   shaders::shader_module vertex{cord->device};
		   vertex.init(SHADER_PATH_BASE_VERTEX);

		   shaders::shader_module fragment{cord->device};
		   fragment.init(SHADER_PATH_BASE_FRAGMENT);


	       VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	       vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	       vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	       vertShaderStageInfo.module = vertex.module;
	       vertShaderStageInfo.pName = "main";

	       VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	       fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	       fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	       fragShaderStageInfo.module = fragment.module;
	       fragShaderStageInfo.pName = "main";

	       VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

	       VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	       vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	       auto bindingDescription = Vertex::getBindingDescription();
	       auto attributeDescriptions = Vertex::getAttributeDescriptions();

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
	       rasterizer.lineWidth = 10.0f;
	       rasterizer.cullMode = VK_CULL_MODE_NONE;
	       rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	       rasterizer.depthBiasEnable = VK_FALSE;

	       VkPipelineMultisampleStateCreateInfo multisampling{};
	       multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	       multisampling.sampleShadingEnable = VK_FALSE;
	       multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	//       VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	//       colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	//       colorBlendAttachment.blendEnable = VK_FALSE;

	//       VkPipelineColorBlendStateCreateInfo colorBlending{};
	//       colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	//       colorBlending.logicOpEnable = VK_FALSE;
	//       colorBlending.logicOp = VK_LOGIC_OP_COPY;
	//       colorBlending.attachmentCount = 1;
	//       colorBlending.pAttachments = &colorBlendAttachment;
	//       colorBlending.blendConstants[0] = 0.0f;
	//       colorBlending.blendConstants[1] = 0.0f;
	//       colorBlending.blendConstants[2] = 0.0f;
	//       colorBlending.blendConstants[3] = 0.0f;

	       VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		   colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		   colorBlendAttachment.blendEnable = VK_TRUE;
		   colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		   colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		   colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		   colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		   colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		   colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	       VkPipelineColorBlendStateCreateInfo colorBlending = {};
	       colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	       colorBlending.logicOpEnable = VK_FALSE;
	       colorBlending.logicOp = VK_LOGIC_OP_COPY;
	       colorBlending.attachmentCount = 1;
	       colorBlending.pAttachments = &colorBlendAttachment;



//	       VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//			colorBlendAttachment.blendEnable = VK_FALSE;
//
//
//			VkPipelineColorBlendStateCreateInfo colorBlending{};
//			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//			colorBlending.logicOpEnable = VK_FALSE;
//			colorBlending.logicOp = VK_LOGIC_OP_COPY;
//			colorBlending.attachmentCount = 1;
//			colorBlending.pAttachments = &colorBlendAttachment;
//			colorBlending.blendConstants[0] = 0.0f;
//			colorBlending.blendConstants[1] = 0.0f;
//			colorBlending.blendConstants[2] = 0.0f;
//			colorBlending.blendConstants[3] = 0.0f;




	//		VkStencilOpState stencilOpState = {};
	//		stencilOpState.failOp = VK_STENCIL_OP_KEEP; // Операция при неудачном стенсильном тесте
	//		stencilOpState.passOp = VK_STENCIL_OP_REPLACE; // Операция при успешном стенсильном тесте и прохождении глубинного теста
	//		stencilOpState.depthFailOp = VK_STENCIL_OP_KEEP; // Операция при успешном стенсильном тесте, но не прохождении глубинного теста
	//		stencilOpState.compareOp = VK_COMPARE_OP_ALWAYS; // Операция сравнения для стенсильного теста
	//		stencilOpState.compareMask = 0xFF; // Маска сравнения для стенсильного теста
	//		stencilOpState.writeMask = 0xFF; // Маска записи для стенсильного буфера
	//		stencilOpState.reference = 0x01; // Значение ссылки для стенсильного теста


			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.stencilTestEnable = VK_FALSE;
			//depthStencil.front = stencilOpState;
			//depthStencil.back = stencilOpState;


	       std::vector<VkDynamicState> dynamicStates = {
	           VK_DYNAMIC_STATE_VIEWPORT,
	           VK_DYNAMIC_STATE_SCISSOR
	       };
	       VkPipelineDynamicStateCreateInfo dynamicState{};
	       dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	       dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	       dynamicState.pDynamicStates = dynamicStates.data();

	       VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	       pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	       pipelineLayoutInfo.setLayoutCount = 1;
	       pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	       if (vkCreatePipelineLayout(cord->device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
	           throw std::runtime_error("failed to create pipeline layout!");
	       }

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
	       pipelineInfo.pDynamicState = &dynamicState;
	       pipelineInfo.layout = pipelineLayout;
	       pipelineInfo.renderPass = renderpass;
	       pipelineInfo.pDepthStencilState = &depthStencil;
	       pipelineInfo.subpass = 0;
	       pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	       if (vkCreateGraphicsPipelines(cord->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
	           throw std::runtime_error("failed to create graphics pipeline!");
	       }
	       return std::make_pair(graphicsPipeline,pipelineLayout);
	//       vkDestroyShaderModule(device, fragShaderModule, nullptr);
	//       vkDestroyShaderModule(device, vertShaderModule, nullptr);
	   }

	   VkDescriptorPool createDescriptorPool(std::shared_ptr<vulkan_spinal_cord> cord) {
		   VkDescriptorPool descriptorPool;
		   VkDescriptorPoolSize poolSize{};
		   poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		   poolSize.descriptorCount = 1;

		   VkDescriptorPoolCreateInfo poolInfo{};
		   poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		   poolInfo.poolSizeCount = 1;
		   poolInfo.pPoolSizes = &poolSize;
		   poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

		   if (vkCreateDescriptorPool(cord->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			   throw std::runtime_error("failed to create descriptor pool!");
		   }
		   return descriptorPool;
	   }

	   std::vector<VkDescriptorSet> createDescriptorSets(std::shared_ptr<vulkan_spinal_cord> cord, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkBuffer uniformBuffer) {

		   std::vector<VkDescriptorSet> descriptorSets;
		   VkDescriptorSetAllocateInfo allocInfo{};
		   allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		   allocInfo.descriptorPool = descriptorPool;
		   allocInfo.descriptorSetCount = 1;//static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		   allocInfo.pSetLayouts = &descriptorSetLayout;

		   descriptorSets.resize(1);
		   if (vkAllocateDescriptorSets(cord->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			   throw std::runtime_error("failed to allocate descriptor sets!");
		   }
//
//		   for (size_t i = 0; i < 1; i++) {
//	           VkDescriptorBufferInfo bufferInfo{};
//	           bufferInfo.buffer = uniformBuffer;
//	           bufferInfo.offset = 0;
//	           bufferInfo.range = sizeof(UniformBufferObject);
//
//			   VkWriteDescriptorSet descriptorWrite{};
//			   descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			   descriptorWrite.dstSet = descriptorSets[i];
//			   descriptorWrite.dstBinding = 0;
//			   descriptorWrite.dstArrayElement = 0;
//			   descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			   descriptorWrite.descriptorCount = 1;
//			   descriptorWrite.pBufferInfo = &bufferInfo;
//
//			   vkUpdateDescriptorSets(cord->device, 1, &descriptorWrite, 0, nullptr);
//		   }
		   return descriptorSets;
	   }



	void Material::perform(VkCommandBuffer cmd, Shape* shape, uint32_t vertex_offset){
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		if(desc_set)
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &desc_set, 0, nullptr);

		vkCmdDrawIndexed(cmd, shape->indices_size, 1, 0, vertex_offset, 0);
	}

	void Material::load(tinygltf::Model& model, tinygltf::Primitive& prim, load_package pack){
		VkDescriptorSetLayout descriptor_set_layout = createDescriptorSetLayout(pack.r_context_ptr->spinal_cord);
		VkDescriptorPool pool = createDescriptorPool(pack.r_context_ptr->spinal_cord);
		std::vector<VkDescriptorSet> sets = createDescriptorSets(pack.r_context_ptr->spinal_cord, pool, descriptor_set_layout, VK_NULL_HANDLE);


		std::pair<VkPipeline,VkPipelineLayout> pipe = createGraphicsPipeline(pack.r_context_ptr->spinal_cord, descriptor_set_layout, pack.r_context_ptr->render_pass);
		pipeline = pipe.first;
		pipelineLayout = pipe.second;
		desc_set = sets[0];

	};
	void Material::r_init(renderer_context* r_context){

	};
}


