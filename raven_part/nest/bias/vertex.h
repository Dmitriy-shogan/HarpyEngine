/*
 * vertex.h
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_BIAS_VERTEX_H_
#define RAVEN_PART_NEST_BIAS_VERTEX_H_
#include <spinal_cord/vulkan_spinal_cord.h>

struct __attribute__((packed)) Vertex {
	    glm::vec3 pos;
	    float __fill = 1337.0f;
	    glm::vec3 color;
	    float __fill2 = 1488.0f;

	    Vertex(glm::vec3 pos, glm::vec3 color){
	    	this->pos = pos;
	    	this->color = color;
	    }

	    static VkVertexInputBindingDescription getBindingDescription() {
	        VkVertexInputBindingDescription bindingDescription{};
	        bindingDescription.binding = 0;
	        bindingDescription.stride = sizeof(Vertex);
	        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	        return bindingDescription;
	    }

	    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
	        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

	        attributeDescriptions[0].binding = 0;
	        attributeDescriptions[0].location = 0;
	        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	        attributeDescriptions[0].offset = offsetof(Vertex, pos);

	        attributeDescriptions[1].binding = 0;
	        attributeDescriptions[1].location = 1;
	        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	        attributeDescriptions[1].offset = offsetof(Vertex, color);

	        return attributeDescriptions;
	    }


	};




#endif /* RAVEN_PART_NEST_BIAS_VERTEX_H_ */
