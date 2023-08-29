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
		void init(std::shared_ptr<harpy::nest::renderer_context> r_context){
			VkDescriptorSetLayout descriptor_set_layout = createDescriptorSetLayout(cord);
			VkDescriptorPool pool = createDescriptorPool(cord);
			std::vector<VkDescriptorSet> sets = createDescriptorSets(cord, pool, descriptor_set_layout, ubo.first[0]);


			std::pair<VkPipeline,VkPipelineLayout> pipe = createGraphicsPipeline(cord, descriptor_set_layout, r_context_ptr->render_pass);
			pipeline = pipe.first;
			pipelineLayout = pipe.second;
			desc_set = sets[0];

		};
		void perform(VkCommandBuffer cmd, Shape* shape);
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_MATERIAL_H_ */
