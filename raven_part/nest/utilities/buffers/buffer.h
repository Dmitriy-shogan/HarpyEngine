/*
 * buffer.h
 *
 *  Created on: 30 Aug 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_
#define RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_

#include <utilities/utilities.h>

namespace harpy::nest{
	struct vulkan_spinal_cord;
}

namespace harpy::utilities{
	std::pair<std::pair<VkBuffer,VkDeviceMemory>, uint32_t> loadVertexBuffer(
				std::shared_ptr<harpy::nest::vulkan_spinal_cord> cord,
				VkCommandBuffer copy_buf,
				VkQueue copy_queue,
				tinygltf::Primitive prim,
				tinygltf::Model model
				);
	std::pair<std::pair<VkBuffer,VkDeviceMemory>, uint32_t> loadIndexBuffer(
				std::shared_ptr<harpy::nest::vulkan_spinal_cord> cord,
				VkCommandBuffer copy_buf,
				VkQueue copy_queue,
				tinygltf::Primitive prim,
				tinygltf::Model model
			);
}



#endif /* RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_ */
