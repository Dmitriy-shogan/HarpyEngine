/*
 * Object.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_OBJECT_H_
#define RAVEN_PART_RESOURCE_TYPES_OBJECT_H_

#include <spinal_cord/vulkan_spinal_cord.h>
#include <memory>
using namespace harpy::nest;
namespace harpy::raven_part::resource_types{

	struct Object{
		std::shared_ptr<nest::vulkan_spinal_cord> cord;

		VkDescriptorSet desc_set;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_OBJECT_H_ */
