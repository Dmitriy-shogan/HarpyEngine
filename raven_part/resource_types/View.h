/*
 * View.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_VIEW_H_
#define RAVEN_PART_RESOURCE_TYPES_VIEW_H_

#include <spinal_cord/vulkan_spinal_cord.h>

namespace harpy::raven_part::resource_types{

	struct CameraPushConstants{

	};

	struct View{
		std::shared_ptr<vulkan_spinal_cord> cord;
		VkViewport viewport{};
		VkRect2D scissor{};


		VkPipeline cameraGraphicsPipeline;

		CameraPushConstants cameraPushConstants;

		//set0
		VkDescriptorSet desc_set;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_VIEW_H_ */
