/*
 * buffer.cpp
 *
 *  Created on: 31 Aug 2023
 *      Author: hexi
 */

#include "buffer.h"
#include <bias/vertex.h>
#include <bias/indice.h>

namespace harpy::utilities{




	struct buffer_parse_context{
		uint32_t unit_size;

		uint32_t read_initial_offset;
		uint32_t read_stride;
		uint32_t read_current_offset;

		uint32_t buffer_stride;
		uint32_t buffer_initial_offset;
		uint32_t buffer_current_offset;

		VkFormat format;
		uint32_t type;
		uint32_t componentType;

		std::any default_value;

		public:
		buffer_parse_context(
				std::any default_value,
				uint32_t unit_size,
				uint32_t read_initial_offset,
				uint32_t read_stride,
				uint32_t buffer_initial_offset,
				uint32_t buffer_stride,
				uint32_t type,
				uint32_t componentType,
				VkFormat format
				){
			this->read_initial_offset = read_initial_offset;
			this->read_stride = read_stride;
			read_current_offset = read_initial_offset;

			this->buffer_initial_offset = buffer_initial_offset;
			this->buffer_stride = buffer_stride;
			buffer_current_offset = buffer_initial_offset;

			this->unit_size = unit_size;

			this->type = type;
			this->componentType = componentType;
			this->format = format;
			this->default_value = default_value;
		}

		uint32_t get_read_point(){
			return read_current_offset;
		}

		uint32_t get_buffer_point(){
			return buffer_current_offset;
		}

		void jump(){
			read_current_offset = read_current_offset + read_stride;
			buffer_current_offset = buffer_current_offset + buffer_stride;
		}

		void reset(){
			read_current_offset = read_initial_offset;
			buffer_current_offset = buffer_initial_offset;
		}

	};


	float_t convert_sfloat(uint32_t componentType, char* ptr){
		switch (componentType) {
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				return *((float_t)ptr);
				break;
			default:
				throw harpy_little_error("unsupported componentType for sfloat");
		}
	}

	uint32_t convert_uint(uint32_t componentType, char* ptr){
		switch (componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				return *((uint32_t)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				return *((uint16_t)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				return *((uint8_t)ptr);
				break;
			default:
				throw harpy_little_error("unsupported componentType for uint");
		}
	}

	uint16_t convert_short(uint32_t componentType, char* ptr){
			switch (componentType) {
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
					return *((uint16_t)ptr);
					break;
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
					return *((uint8_t)ptr);
					break;
				default:
					throw harpy_little_error("unsupported componentType for sint");
			}
		}


	void convert(uint32_t componentType, uint32_t type, VkFormat format, char* ptr, std::any default_value){
		//SLOW

		uint32_t current_count = tinygltf::GetNumComponentsInType(type);

		for (int i_comp = 0; i_comp < getChannelCountForFormat(format); ++i_comp) {

			if (i_comp<current_count){
				if (isFloat(format)) {
						(*((float_t*)ptr)) = convert_sfloat(componentType, ptr);
				}else if(isUInt(format)){
						(*((uint32_t*)ptr)) = convert_uint(componentType, ptr);
				}else if(isSInt(format)){
					(*((uint16_t*)ptr)) = convert_short(componentType, ptr);
				}else{
					throw harpy_little_error("unsupported VkFormat to convert ");
				}
			}else{
				if (isFloat(format)) {
						(*((float_t*)ptr)) = (float_t)default_value;
				}else if(isUInt(format)){
						(*((uint32_t*)ptr)) = (uint32_t)default_value;
				}else if(isSInt(format)){
					(*((uint16_t*)ptr)) = (uint16_t)default_value;
				}else{
					throw harpy_little_error("unsupported VkFormat to convert ");
				}
			}

			ptr = ptr + 1;
		}
	}


	void loadPart(std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> s_pair, char* buffer_ptr){
		buffer_ptr = buffer_ptr + s_pair.first.get_buffer_point();
		s_pair.second->seekg(s_pair.first.get_read_point(), std::ios::beg);
		s_pair.second->read(buffer_ptr, s_pair.first.unit_size);
		convert(s_pair.first.componentType, s_pair.first.type, s_pair.first.format, buffer_ptr, s_pair.first.default_value);
		s_pair.first.jump();
	}


	uint32_t getMaxAttributeCount(tinygltf::Model model, tinygltf::Primitive prim){
		uint32_t max_count = 0;
		for (auto &attrib : prim.attributes) {
			max_count = std::max((uint32_t)model.accessors[attrib.second].count, max_count);
		}
		return max_count;
	}


	void loadIndice(std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> s_pair, char* buffer_ptr){

	}


	uint32_t findMemoryType(std::shared_ptr<vulkan_spinal_cord> cord, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	       VkPhysicalDeviceMemoryProperties memProperties;
	       vkGetPhysicalDeviceMemoryProperties(cord->ph_device, &memProperties);

	       for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
	           if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
	               return i;
	           }
	       }

	       throw std::runtime_error("failed to find suitable memory type!");
	   }

	void createBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		   VkBufferCreateInfo bufferInfo{};
		   bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		   bufferInfo.size = size;
		   bufferInfo.usage = usage;
		   bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		   if (vkCreateBuffer(cord->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			   throw std::runtime_error("failed to create buffer!");
		   }

		   VkMemoryRequirements memRequirements;
		   vkGetBufferMemoryRequirements(cord->device, buffer, &memRequirements);

		   VkMemoryAllocateInfo allocInfo{};
		   allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		   allocInfo.allocationSize = memRequirements.size;
		   allocInfo.memoryTypeIndex = findMemoryType(cord,memRequirements.memoryTypeBits, properties);

		   if (vkAllocateMemory(cord->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			   throw std::runtime_error("failed to allocate buffer memory!");
		   }

		   vkBindBufferMemory(cord->device, buffer, bufferMemory, 0);
	   }

	void copyBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue transferQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		   vkResetCommandBuffer(copy_buf,0);

		   VkCommandBufferBeginInfo beginInfo{};
		   beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		   vkBeginCommandBuffer(copy_buf, &beginInfo);

		   VkBufferCopy copyRegion{};
		   copyRegion.size = size;
		   vkCmdCopyBuffer(copy_buf, srcBuffer, dstBuffer, 1, &copyRegion);

		   vkEndCommandBuffer(copy_buf);

		   VkSubmitInfo submitInfo{};
		   submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		   submitInfo.commandBufferCount = 1;
		   submitInfo.pCommandBuffers = &copy_buf;

		   vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		   vkQueueWaitIdle(transferQueue);

	   }

	std::map<std::string, std::shared_ptr<std::ifstream>> initialize_buffer_attrib_ifstreams(
			tinygltf::Model model,
			tinygltf::Primitive prim
	){
		//GLTF DATA URI UNSUPPORTED

		std::map<std::string, std::shared_ptr<std::ifstream>> buffer_map{};
		for (auto &attrib : prim.attributes) {
		    tinygltf::Accessor accessor = model.accessors[attrib.second];
			tinygltf::BufferView buffView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer buff = model.buffers[buffView.buffer];

		    std::shared_ptr<std::ifstream> ifs = std::make_shared<std::ifstream>(std::ifstream{buff.uri, std::ios::ate | std::ios::binary});
		    if (ifs->tellg() == -1) throw harpy_little_error("failed to open bin gltf file!");

			buffer_map[buff.uri] = ifs;
		}
		return buffer_map;
	}

	std::shared_ptr<std::ifstream> initialize_buffer_indices_ifstream(
				tinygltf::Model model,
				tinygltf::Primitive prim
	){
		//GLTF DATA URI UNSUPPORTED

		std::shared_ptr<std::ifstream> ifs;

			tinygltf::Accessor accessor = model.accessors[prim.indices];
			tinygltf::BufferView buffView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer buff = model.buffers[buffView.buffer];

			ifs = std::make_shared<std::ifstream>(std::ifstream{buff.uri, std::ios::ate | std::ios::binary});
			if (ifs->tellg() == -1) throw harpy_little_error("failed to open bin gltf file!");

		return ifs;
	}


	std::pair<VkBuffer,VkDeviceMemory> loadVertexBuffer(
			std::shared_ptr<vulkan_spinal_cord> cord,
			VkCommandBuffer copy_buf,
			VkQueue copy_queue,
			tinygltf::Primitive prim,
			tinygltf::Model model
			) {
		uint32_t vert_max_count = getMaxAttributeCount(model, prim);

		auto attributeDescriptions = Vertex::getAttributeDescriptions();
		auto gltfNames = Vertex::getGLTFNames();
		VkDeviceSize bufferSize = sizeof(Vertex) * vert_max_count;

		std::map<std::string, std::shared_ptr<std::ifstream>> buffer_map = initialize_buffer_attrib_ifstreams(model, prim);

		std::vector<std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>>> attrib_sources{};

		for (uint32_t i_attrib : gltfNames.size()){
			std::string attrib = gltfNames[i_attrib];
			tinygltf::Accessor accessor = model.accessors[prim.attributes[attrib]];
			tinygltf::BufferView buffView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer buff = model.buffers[buffView.buffer];

			std::any default_val{};
			switch (attrib) {
				case "COLOR_0":
					default_val = 1.0f;
					break;
				case "COLOR_1":
					default_val = 1.0f;
					break;
				case "COLOR_2":
					default_val = 1.0f;
					break;
				case "COLOR_3":
					default_val = 1.0f;
					break;
				default:
					default_val = 0.0f;;
			}

			std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> p = std::make_pair(
								buffer_parse_context{
												default_val,
												tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type),
												buffView.byteOffset + accessor.byteOffset,
												buffView.byteStride,
												attributeDescriptions[i_attrib].offset,
												sizeof(Vertex),
												accessor.type,
												accessor.componentType,
												attributeDescriptions[i_attrib].format
								},
								buffer_map[buff.uri]);

			attrib_sources.push_back(p);
		}


		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		char* data;
		vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &(void*)data);
		for (int i_vertex = 0; i_vertex < vert_max_count; ++i_vertex) {
			for (int i_attrib = 0; i_attrib < attrib_sources.size(); ++i_attrib) {
				loadPart(attrib_sources[i_attrib], data);
			}
		}
		//memcpy(data, vertices.data(), (size_t) bufferSize);
		vkUnmapMemory(cord->device, stagingBufferMemory);

		createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
		vkFreeMemory(cord->device, stagingBufferMemory, nullptr);
		return std::make_pair(vertexBuffer, vertexBufferMemory);
   }

	std::pair<VkBuffer,VkDeviceMemory> loadIndexBuffer(
			std::shared_ptr<vulkan_spinal_cord> cord,
			VkCommandBuffer copy_buf,
			VkQueue copy_queue,
			tinygltf::Primitive prim,
			tinygltf::Model model
		) {

		tinygltf::Accessor accessor = model.accessors[prim.indices];
		tinygltf::BufferView buffView = model.bufferViews[accessor.bufferView];
		tinygltf::Buffer buff = model.buffers[buffView.buffer];

		uint32_t ind_max_count = accessor.count;

		VkDeviceSize bufferSize = sizeof(Indice) * ind_max_count;

		std::any default_val{};
		default_val = 0;

		std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> indices_source = std::make_pair(
		buffer_parse_context{
						default_val,
						tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type),
						buffView.byteOffset + accessor.byteOffset,
						buffView.byteStride,
						0,
						sizeof(Vertex),
						accessor.type,
						accessor.componentType,
						Indice::getIndiceFormat()
		},
		initialize_buffer_indices_ifstream(model, prim));


		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		char* data;
		vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &(void*)data);
		for (int i_indice = 0; i_indice < ind_max_count; ++i_indice) {
			loadPart(indices_source, data);
		}
		   //memcpy(data, indices.data(), (size_t) bufferSize);
		vkUnmapMemory(cord->device, stagingBufferMemory);

		createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
		vkFreeMemory(cord->device, stagingBufferMemory, nullptr);
		return std::make_pair(indexBuffer,indexBufferMemory);
	}

//	std::pair<std::vector<VkBuffer>, std::pair<std::vector<VkDeviceMemory>, std::vector<void*>>> loadUniformBuffers(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue) {
//		   VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//		   std::vector<VkBuffer> uniformBuffers;
//		   std::vector<VkDeviceMemory> uniformBuffersMemory;
//		   std::vector<void*> uniformBuffersMapped;
//
//		   uniformBuffers.resize(1);
//		   uniformBuffersMemory.resize(1);
//		   uniformBuffersMapped.resize(1);
//
//		   for (size_t i = 0; i < 1; i++) {
//			   createBuffer(cord, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
//
//			   vkMapMemory(cord-> device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
//		   }
//		   return std::make_pair(uniformBuffers,std::make_pair(uniformBuffersMemory,uniformBuffersMapped));
//
//	}


}
