/*
 * indice.h
 *
 *  Created on: 3 Sept 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_BIAS_INDICE_H_
#define RAVEN_PART_NEST_BIAS_INDICE_H_
#include <spinal_cord/vulkan_spinal_cord.h>

struct __attribute__((packed)) Indice{
	uint16_t p1;
	uint16_t p2;
	uint16_t p3;

	static VkFormat getIndiceFormat(){
		return VK_FORMAT_R32G32B32_SINT;
	}
};


#endif /* RAVEN_PART_NEST_BIAS_INDICE_H_ */
