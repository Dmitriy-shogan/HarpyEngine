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
namespace harpy::raven_part::resource_types{

	struct Shape{
		std::shared_ptr<renderer_context> r_context;

		VkBuffer vertexBuffer = nullptr;
		uint32_t vert_size;
		VkBuffer indexBuffer = nullptr;
		VkIndexType indexType;
		uint32_t indices_size;

		//set4
		VkDescriptorSet desc_set;

		void init(std::shared_ptr<harpy::nest::renderer_context> r_context){

			vertexBuffer = createVertexBuffer2(cord, transfer_queue.first.second, transfer_queue.first.first).first;
			vert_size = vertices2.size();
			indices_size = indices.size();
			indexBuffer = createIndexBuffer(cord, transfer_queue.first.second, transfer_queue.first.first).first;
			indexType = VK_INDEX_TYPE_UINT16;
		};

		void perform(VkCommandBuffer cmd);
	};
}


#endif /* RAVEN_PART_RESOURCE_TYPES_SHAPE_H_ */
