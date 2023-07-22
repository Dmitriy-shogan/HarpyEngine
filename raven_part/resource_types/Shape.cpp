/*
 * Shape.cpp
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#include "Shape.h"
#include <renderer_context/renderer_context.h>

namespace harpy::raven_part::resource_types{
	void Shape::perform(VkCommandBuffer cmd){
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, offsets);
		vkCmdBindIndexBuffer(cmd, indexBuffer, 0, indexType);
	}

}
