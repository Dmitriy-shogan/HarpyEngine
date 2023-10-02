/*
 * Shape.cpp
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#include "Shape.h"
#include <renderer_context/renderer_context.h>
#include <raven_part.h>
#include <bias/vertex.h>
#include <bias/indice.h>

namespace harpy::raven_part::resource_types{
	void Shape::perform(VkCommandBuffer cmd){
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.first, offsets);
		vkCmdBindIndexBuffer(cmd, indexBuffer.first, 0, indexType);
	}

	void Shape::load(tinygltf::Model& model, tinygltf::Primitive& prim, harpy::raven_part::load_package pack){

		std::cout<<"Shape::load"<<std::endl;
		auto indexBufferRes = harpy::utilities::loadIndexBuffer(pack.r_context_ptr->spinal_cord, pack.copy_buf, pack.copy_queue, prim, model);
		std::cout<<"Shape::load1"<<std::endl;
		auto vertexBufferRes = harpy::utilities::loadVertexBuffer(pack.r_context_ptr->spinal_cord, pack.copy_buf, pack.copy_queue, prim, model);
		std::cout<<"Shape::load2"<<std::endl;
		vertexBuffer = vertexBufferRes.first;
		indexBuffer = indexBufferRes.first;

		vert_size = vertexBufferRes.second;
		indices_size = indexBufferRes.second;
		indexType = harpy::nest::Indice::getType();
	};

}
