/*
 * harpy.cpp
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */
#include "harpy.h"
#include <memory>
#include <shaders/shader_module.h>
#include <spinal_cord/vulkan_spinal_cord.h>

namespace harpy{
	std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;


	struct UniformBufferObject {
	    alignas(16) glm::mat4 model;
	    alignas(16) glm::mat4 view;
	    alignas(16) glm::mat4 proj;
	};


void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* phys_cond){ //, std::vector<human_part::ECS::Entity*> entities
	int ang = 0;
	glm::vec3 axisX(1.0f, 0.0f, 0.0f);
	glm::vec3 axisY(0.0f, 1.0f, 0.0f);
		while(phys_cond->test_and_set(std::memory_order_acquire)){
			ang += 1;
			ang = ang % 360;
			float angleRadians = glm::radians(ang / 1.0);
			//glm::mat4 r_mat = glm::mat4(1.0f), angleRadians, glm::vec3(0.0f, 1.0f, 0.0f));

			obj_str_ptr->lock.lock();
			obj_str_ptr->consumed.clear();
			obj_str_ptr->entities = std::make_shared<std::vector<human_part::ECS::Entity*>>();
			obj_str_ptr->entities->push_back(entities[0]);
			//obj_str_ptr->entities->push_back(entities[1]);

			dynamic_cast<harpy::human_part::ECS::Transform*>(obj_str_ptr->camera->get_components_by_name(harpy::human_part::ECS::Transform::name)[0])->rot = glm::angleAxis(angleRadians, axisX);

			dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[0]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0])->rot = glm::angleAxis(angleRadians, axisY);


			//harpy::human_part::ECS::Transform* tr1 = dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[0]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);
			//tr1->pos_mat = glm::rotate(tr1->pos_mat, glm::radians(1.0f), axis);
			//harpy::human_part::ECS::Transform* tr2 = dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[1]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);
			//tr2->pos_mat = glm::rotate(tr2->pos_mat, glm::radians(1.0f), axis);

			obj_str_ptr->lock.unlock();
			std::this_thread::sleep_for(sleepDuration);
		}

	}


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

       VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;


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
   void copyBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue transferQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
   void createBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
   std::pair<VkBuffer,VkDeviceMemory> createVertexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue) {
	   VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	   VkBuffer vertexBuffer;
	   VkDeviceMemory vertexBufferMemory;

	   VkBuffer stagingBuffer;
	   VkDeviceMemory stagingBufferMemory;
	   createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	   void* data;
	   vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		   memcpy(data, vertices.data(), (size_t) bufferSize);
	   vkUnmapMemory(cord->device, stagingBufferMemory);

	   createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	   copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, vertexBuffer, bufferSize);

	   vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
	   vkFreeMemory(cord->device, stagingBufferMemory, nullptr);
	   return std::make_pair(vertexBuffer, vertexBufferMemory);
   }


   std::pair<VkBuffer,VkDeviceMemory> createVertexBuffer2(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue) {
   	   VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

   	   VkBuffer vertexBuffer;
   	   VkDeviceMemory vertexBufferMemory;

   	   VkBuffer stagingBuffer;
   	   VkDeviceMemory stagingBufferMemory;
   	   createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

   	   void* data;
   	   vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &data);
   		   memcpy(data, vertices2.data(), (size_t) bufferSize);
   	   vkUnmapMemory(cord->device, stagingBufferMemory);

   	   createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT  | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

   	   copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, vertexBuffer, bufferSize);

   	   vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
   	   vkFreeMemory(cord->device, stagingBufferMemory, nullptr);
   	   return std::make_pair(vertexBuffer, vertexBufferMemory);
      }

   std::pair<VkBuffer,VkDeviceMemory> createIndexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue) {
	   VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	   VkBuffer indexBuffer;
	   VkDeviceMemory indexBufferMemory;

	   VkBuffer stagingBuffer;
	   VkDeviceMemory stagingBufferMemory;
	   createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	   void* data;
	   vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &data);
		   memcpy(data, indices.data(), (size_t) bufferSize);
	   vkUnmapMemory(cord->device, stagingBufferMemory);

	   createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	   copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, indexBuffer, bufferSize);

	   vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
	   vkFreeMemory(cord->device, stagingBufferMemory, nullptr);
	   return std::make_pair(indexBuffer,indexBufferMemory);
   }

   std::pair<std::vector<VkBuffer>, std::pair<std::vector<VkDeviceMemory>, std::vector<void*>>> createUniformBuffers(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue) {
	   VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	   std::vector<VkBuffer> uniformBuffers;
	   std::vector<VkDeviceMemory> uniformBuffersMemory;
	   std::vector<void*> uniformBuffersMapped;

	   uniformBuffers.resize(1);
	   uniformBuffersMemory.resize(1);
	   uniformBuffersMapped.resize(1);

	   for (size_t i = 0; i < 1; i++) {
		   createBuffer(cord, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);

		   vkMapMemory(cord-> device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	   }
	   return std::make_pair(uniformBuffers,std::make_pair(uniformBuffersMemory,uniformBuffersMapped));

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

	   for (size_t i = 0; i < 1; i++) {
           VkDescriptorBufferInfo bufferInfo{};
           bufferInfo.buffer = uniformBuffer;
           bufferInfo.offset = 0;
           bufferInfo.range = sizeof(UniformBufferObject);

		   VkWriteDescriptorSet descriptorWrite{};
		   descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		   descriptorWrite.dstSet = descriptorSets[i];
		   descriptorWrite.dstBinding = 0;
		   descriptorWrite.dstArrayElement = 0;
		   descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		   descriptorWrite.descriptorCount = 1;
		   descriptorWrite.pBufferInfo = &bufferInfo;

		   vkUpdateDescriptorSets(cord->device, 1, &descriptorWrite, 0, nullptr);
	   }
	   return descriptorSets;
   }
   uint32_t findMemoryType(std::shared_ptr<vulkan_spinal_cord> cord, uint32_t typeFilter, VkMemoryPropertyFlags properties) ;
   void createBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	   VkBufferCreateInfo bufferInfo{};
	   bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	   bufferInfo.size = size;
	   bufferInfo.usage = usage;
	   bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	   if (vkCreateBuffer(cord->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		   throw std::runtime_error("failed to create buffer!");
	   }

	   VkMemoryRequirements memRequirements;
	   vkGetBufferMemoryRequirements(cord->device, buffer, &memRequirements);

	   VkMemoryAllocateInfo allocInfo{};
	   allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	   allocInfo.allocationSize = memRequirements.size;
	   allocInfo.memoryTypeIndex = findMemoryType(cord,memRequirements.memoryTypeBits, properties);

	   if (vkAllocateMemory(cord->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		   throw std::runtime_error("failed to allocate buffer memory!");
	   }

	   vkBindBufferMemory(cord->device, buffer, bufferMemory, 0);
   }

   uint32_t findMemoryType(std::shared_ptr<vulkan_spinal_cord> cord, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
       VkPhysicalDeviceMemoryProperties memProperties;
       vkGetPhysicalDeviceMemoryProperties(cord->ph_device, &memProperties);

       for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
           if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
               return i;
           }
       }

       throw std::runtime_error("failed to find suitable memory type!");
   }

   void copyBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue transferQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	   vkResetCommandBuffer(copy_buf,0);

	   VkCommandBufferBeginInfo beginInfo{};
	   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	   vkBeginCommandBuffer(copy_buf, &beginInfo);

	   VkBufferCopy copyRegion{};
	   copyRegion.size = size;
	   vkCmdCopyBuffer(copy_buf, srcBuffer, dstBuffer, 1, &copyRegion);

	   vkEndCommandBuffer(copy_buf);

	   VkSubmitInfo submitInfo{};
	   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	   submitInfo.commandBufferCount = 1;
	   submitInfo.pCommandBuffers = &copy_buf;

	   vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	   vkQueueWaitIdle(transferQueue);

   }


   void updateUniformBuffer(VkExtent2D extent, std::vector<void*> uniformBuffersMapped) {
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo{};
		ubo.model = glm::mat4(1.0f);
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(90.0f), extent.width / static_cast<float>(extent.height), 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		memcpy(uniformBuffersMapped[0], &ubo, sizeof(ubo));
   }


   void render(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* rend_cond){
	   std::cout<<"void render"<<std::endl;

	   r_context_ptr->render_loop(obj_str_ptr, rend_cond);
   	}


}
