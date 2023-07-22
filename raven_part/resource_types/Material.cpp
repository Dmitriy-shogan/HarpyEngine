/*
 * Material.cpp
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#include "Material.h"
#include <renderer_context/renderer_context.h>

namespace harpy::raven_part::resource_types{

	void Material::perform(VkCommandBuffer cmd, Shape* shape){
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		if(desc_set)
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &desc_set, 0, nullptr);

		vkCmdDrawIndexed(cmd, shape->indices_size, 1, 0, 0, 0);
	}
}


