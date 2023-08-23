/*
 * Material.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_MATERIAL_H_
#define RAVEN_PART_RESOURCE_TYPES_MATERIAL_H_
#include <spinal_cord/vulkan_spinal_cord.h>
#include <resource_types/Shape.h>

namespace harpy::nest{
	struct renderer_context;
}

namespace harpy::raven_part::resource_types{

	struct Material{
		std::shared_ptr<renderer_context> r_context;
		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;

		//set1
		VkDescriptorSet desc_set;
		void init(std::shared_ptr<harpy::nest::renderer_context> r_context){};
		void perform(VkCommandBuffer cmd, Shape* shape);
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_MATERIAL_H_ */
