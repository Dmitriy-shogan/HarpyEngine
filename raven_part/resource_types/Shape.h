/*
 * Object.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_SHAPE_H_
#define RAVEN_PART_RESOURCE_TYPES_SHAPE_H_

#include <spinal_cord/vulkan_spinal_cord.h>
#include <memory>
using namespace harpy::nest;
namespace harpy::raven_part::resource_types{

	struct Shape{
		std::shared_ptr<nest::vulkan_spinal_cord> cord;

		VkBuffer vertexBuffer = nullptr;
		VkBuffer indexBuffer = nullptr;
		VkIndexType indexType;
		uint32_t indices_size;

		//set4
		VkDescriptorSet desc_set;
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_SHAPE_H_ */
