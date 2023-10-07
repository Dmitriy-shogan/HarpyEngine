/*
 * View.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_VIEW_H_
#define RAVEN_PART_RESOURCE_TYPES_VIEW_H_


#include <shaders/shader_module.h>

#include <bias/vertex.h>
#include <ECS/ECS.h>
#include <resource_types/Shape.h>

#include <spinal_cord/vulkan_spinal_cord.h>

namespace harpy::nest{
	struct renderer_context;
}

namespace harpy::raven_part{
	struct load_package;
}

namespace harpy::raven_part::resource_types{
	struct View{
		enum CameraType{
			PRESPECTIVE,
			ORTHOGRAPHIC
		};

		struct CameraPushConstants{
			glm::mat4 transform;
			glm::vec4 translate;

		};



		std::shared_ptr<harpy::nest::renderer_context> r_context;
		VkViewport viewport{};
		VkRect2D scissor{};
		glm::vec2 view_field;

		void load(tinygltf::Model& model, harpy::raven_part::load_package pack);

		void view_perform(VkCommandBuffer cmd);
		void camera_perform(
				VkCommandBuffer cmd,
				VkDescriptorSet desc_set,
				std::pair<VkBuffer, VkDeviceSize> vert_tmp,//buffer,offset
				Shape* shape,
				human_part::ECS::Transform* camera,
				human_part::ECS::Transform* object);

		//set0

		VkDescriptorSetLayout desc_set_layout;

		VkPipeline cameraGraphicsPipeline;
		VkPipelineLayout cameraPipelineLayout;

		CameraType cameraType;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_VIEW_H_ */
