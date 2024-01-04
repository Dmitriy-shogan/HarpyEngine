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
#include "../../supply/supply.h"
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
			PERSPECTIVE,
			ORTHOGRAPHIC,
			ISOMETRIC
		};

		struct CameraPushConstants{
			alignas(16) glm::mat4 transform;
			alignas(16) glm::vec4 translate;

		};



		std::shared_ptr<harpy::nest::renderer_context> r_context;
		VkViewport viewport{};
		VkRect2D scissor{};
		glm::vec2 view_field;

		float fov;
		float aspect; // Соотношение сторон
		float near;      // Ближняя отсекающая плоскость
		float far; // Дальняя отсекающая плоскость

		float xmag;
		float ymag;


		void load(tinygltf::Model& model, harpy::raven_part::load_package pack);
		void r_init(renderer_context* r_context);
		void view_perform(VkCommandBuffer cmd);
		void camera_perform(
				VkCommandBuffer cmd,
				std::pair<VkBuffer, VkDeviceSize> desc_set,
				uint32_t region_i,//buffer,offset
				Shape* shape,
				transform camera,
				transform object);

		//set0



		VkPipeline cameraGraphicsPipeline;
		VkPipelineLayout cameraPipelineLayout;

		CameraType cameraType;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_VIEW_H_ */
