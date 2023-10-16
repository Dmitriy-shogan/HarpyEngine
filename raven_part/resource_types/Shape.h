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


namespace harpy::nest{
	struct renderer_context;
}
namespace harpy::raven_part{
	struct load_package;
}

namespace harpy::raven_part::resource_types{

	struct Shape{
		std::shared_ptr<renderer_context> r_context;

		std::pair<VkBuffer, VkDeviceMemory> vertexBuffer {};
		uint32_t vert_size;
		std::pair<VkBuffer, VkDeviceMemory>  indexBuffer {};
		VkIndexType indexType;
		uint32_t indices_size;

		//set4
		VkDescriptorSet desc_set;

		void load(tinygltf::Model& model, tinygltf::Primitive& prim, harpy::raven_part::load_package pack);
		void r_init(renderer_context* r_context);

		void perform(VkCommandBuffer cmd);
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_SHAPE_H_ */
