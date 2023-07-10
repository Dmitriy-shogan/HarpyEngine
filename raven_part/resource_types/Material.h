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

namespace harpy::raven_part::resource_types{

	struct Material{
		std::shared_ptr<vulkan_spinal_cord> cord;

		VkDescriptorSet desc_set;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_MATERIAL_H_ */