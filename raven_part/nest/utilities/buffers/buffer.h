/*
 * buffer.h
 *
 *  Created on: 30 Aug 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_
#define RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_

#import <spinal_cord/vulkan_spinal_cord.h>

namespace harpy::utilities{
	std::pair<VkBuffer,VkDeviceMemory> loadVertexBuffer(std::shared_ptr<harpy::nest::vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
	std::pair<VkBuffer,VkDeviceMemory> loadIndexBuffer(std::shared_ptr<harpy::nest::vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);

}



#endif /* RAVEN_PART_NEST_UTILITIES_BUFFERS_BUFFER_H_ */
