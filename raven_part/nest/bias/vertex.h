/*
 * vertex.h
 *
 *  Created on: Jul 19, 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_NEST_BIAS_VERTEX_H_
#define RAVEN_PART_NEST_BIAS_VERTEX_H_
#include <spinal_cord/vulkan_spinal_cord.h>
namespace harpy::nest{
	struct Vertex {

		alignas(16) glm::vec3 POSITION;
		alignas(16) glm::vec3 NORMAL;
		alignas(16) glm::vec4 TANGENT;
		alignas(16) glm::vec4 CUSTOM;

		alignas(8) glm::vec2 TEXCOORD_0;
		alignas(8) glm::vec2 TEXCOORD_1;
		alignas(8) glm::vec2 TEXCOORD_2;
		alignas(8) glm::vec2 TEXCOORD_3;

		alignas(16) glm::vec4 COLOR_0;
		alignas(16) glm::vec4 COLOR_1;
		alignas(16) glm::vec4 COLOR_2;
		alignas(16) glm::vec4 COLOR_3;

		alignas(16) glm::uvec4 JOINTS_0;
		alignas(16) glm::uvec4 JOINTS_1;
		alignas(16) glm::uvec4 JOINTS_2;
		alignas(16) glm::uvec4 JOINTS_3;

		alignas(16) glm::vec4 WEIGHTS_0;
		alignas(16) glm::vec4 WEIGHTS_1;
		alignas(16) glm::vec4 WEIGHTS_2;
		alignas(16) glm::vec4 WEIGHTS_3;




			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 20> getAttributeDescriptions() {
				//GREATER OFFSET SEQUENCE MATTER

				std::array<VkVertexInputAttributeDescription, 20> attributeDescriptions{};
				uint32_t i = 0;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, POSITION);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, NORMAL);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, TANGENT);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, CUSTOM);
				i++;

				//TEXCORD CHANNELS

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, TEXCOORD_0);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, TEXCOORD_1);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, TEXCOORD_2);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, TEXCOORD_3);
				i++;

				//COLOR CHANNELS

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, COLOR_0);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, COLOR_1);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, COLOR_2);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, COLOR_3);
				i++;

				//JOINTS CHANNELS

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
				attributeDescriptions[i].offset = offsetof(Vertex, JOINTS_0);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
				attributeDescriptions[i].offset = offsetof(Vertex, JOINTS_1);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
				attributeDescriptions[i].offset = offsetof(Vertex, JOINTS_2);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_UINT;
				attributeDescriptions[i].offset = offsetof(Vertex, JOINTS_3);
				i++;


				//WEIGHTS CHANNELS

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, WEIGHTS_0);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, WEIGHTS_1);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, WEIGHTS_2);
				i++;

				attributeDescriptions[i].binding = 0;
				attributeDescriptions[i].location = i;
				attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[i].offset = offsetof(Vertex, WEIGHTS_3);
				i++;

				return attributeDescriptions;
			}

			static std::vector<std::string> getGLTFNames(){
				//GREATER OFFSET SEQUENCE MATTER
				return std::vector<std::string>{
					"POSITION",
					"NORMAL",
					"TANGENT",
					"CUSTOM",
					"TEXCOORD_0",
					"TEXCOORD_1",
					"TEXCOORD_2",
					"TEXCOORD_3",
					"COLOR_0",
					"COLOR_1",
					"COLOR_2",
					"COLOR_3",
					"JOINTS_0",
					"JOINTS_1",
					"JOINTS_2",
					"JOINTS_3",
					"WEIGHTS_0",
					"WEIGHTS_1",
					"WEIGHTS_2",
					"WEIGHTS_3",
				};
			}
		};
}



#endif /* RAVEN_PART_NEST_BIAS_VERTEX_H_ */
