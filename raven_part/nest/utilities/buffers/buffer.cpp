/*
 * buffer.cpp
 *
 *  Created on: 31 Aug 2023
 *      Author: hexi
 */

#include <bias/vertex.h>
#include <bias/indice.h>
#include <spinal_cord/vulkan_spinal_cord.h>

#include "buffer.h"


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

		double norm_factor;

		public:
		buffer_parse_context(
				std::any default_value,
				double norm_factor,
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

			this->norm_factor = norm_factor;
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


	float_t convert_float(uint32_t componentType, char* ptr){
		switch (componentType) {
			case TINYGLTF_COMPONENT_TYPE_FLOAT:
				return *((float_t*)ptr);
				break;
			default:
				throw harpy_little_error("unsupported componentType for sfloat");
		}
	}

	uint32_t convert_uint(uint32_t componentType, char* ptr){
		switch (componentType) {
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
				return *((uint32_t*)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
				return *((uint16_t*)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
				return *((uint8_t*)ptr);
				break;
			default:
				throw harpy_little_error("unsupported componentType for uint");
		}
	}

	int32_t convert_int(uint32_t componentType, char* ptr){
		switch (componentType) {
			case TINYGLTF_COMPONENT_TYPE_INT:
				return *((int32_t*)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_SHORT:
				return *((int16_t*)ptr);
				break;
			case TINYGLTF_COMPONENT_TYPE_BYTE:
				return *((int8_t*)ptr);
				break;
			default:
				throw harpy_little_error("unsupported componentType for uint");
		}

	}
	void convert(std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>>& s_pair, char* ptr){
		//SLOW
		uint32_t current_count = tinygltf::GetNumComponentsInType(s_pair.first.type);
		char* s_ptr = ptr;
		uint32_t comp_size = getVulkanComponentSize(s_pair.first.format);
		uint32_t comp_count = getChannelCountForFormat(s_pair.first.format);
		ptr = ptr + comp_size * comp_count;
		s_ptr = s_ptr + s_pair.first.unit_size;

		for (int32_t i_comp = comp_count - 1; i_comp >= 0 ; --i_comp) {
			ptr = ptr - comp_size;
			s_ptr = s_ptr - tinygltf::GetComponentSizeInBytes(s_pair.first.componentType);
			if (i_comp<current_count){

				if (isFloat(s_pair.first.format)) {

					if (comp_size == 1){
						goto conv_err;
					}else if(comp_size == 2){
						goto conv_err;
					}else if(comp_size == 4){
						auto t_ptr = ((float_t*)ptr);
						*t_ptr = convert_float(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}

				}else if(isUInt(s_pair.first.format)){
					if (comp_size == 1){
						auto t_ptr = ((uint8_t*)ptr);
						*t_ptr = convert_uint(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}else if(comp_size == 2){
						auto t_ptr = ((uint16_t*)ptr);
						*t_ptr = convert_uint(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}else if(comp_size == 4){
						auto t_ptr = ((uint32_t*)ptr);
						*t_ptr = convert_uint(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}
				}else if(isSInt(s_pair.first.format)){
					if (comp_size == 1){
						auto t_ptr = ((int8_t*)ptr);
						*t_ptr = convert_int(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}else if(comp_size == 2){
						auto t_ptr = ((int16_t*)ptr);
						*t_ptr = convert_int(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}else if(comp_size == 4){
						auto t_ptr = ((int32_t*)ptr);
						*t_ptr = convert_int(s_pair.first.componentType, s_ptr) / s_pair.first.norm_factor;
					}

				}else{
					goto conv_err;
				}
			}else{
				if (isFloat(s_pair.first.format)) {
					if (comp_size == 1){
						goto conv_err;
					}else if(comp_size == 2){
						goto conv_err;
					}else if(comp_size == 4){
						auto t_ptr = ((float_t*)ptr);
						*t_ptr = std::any_cast<float_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}

				}else if(isUInt(s_pair.first.format)){
					if (comp_size == 1){
						auto t_ptr = ((uint8_t*)ptr);
						*t_ptr = std::any_cast<uint32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}else if(comp_size == 2){
						auto t_ptr = ((uint16_t*)ptr);
						*t_ptr = std::any_cast<uint32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}else if(comp_size == 4){
						auto t_ptr = ((uint32_t*)ptr);
						*t_ptr = std::any_cast<uint32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}
				}else if(isSInt(s_pair.first.format)){
					if (comp_size == 1){
						auto t_ptr = ((int8_t*)ptr);
						*t_ptr = std::any_cast<int32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}else if(comp_size == 2){
						auto t_ptr = ((int16_t*)ptr);
						*t_ptr = std::any_cast<int32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}else if(comp_size == 4){
						auto t_ptr = ((int32_t*)ptr);
						*t_ptr = std::any_cast<int32_t>(s_pair.first.default_value) / s_pair.first.norm_factor;
					}

				}else{
					goto conv_err;
				}
			}


		}
		return;

		conv_err:
		throw harpy_little_error("unsupported VkFormat to convert ");
	}


	void loadPart(std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>>& s_pair, char* buffer_ptr){
		//std::cout<<"loadPart"<<std::endl;
		char* buffer_ptr1 = buffer_ptr + s_pair.first.get_buffer_point();
		uint32_t point = s_pair.first.get_read_point();
//		std::cout<<"point: "<<point<<std::endl;
//		std::cout<<"unit_size: "<<s_pair.first.unit_size<<std::endl;
//		std::cout<<"read stride: "<<s_pair.first.read_stride<<std::endl;
//		std::cout<<"stream: "<<s_pair.second<<std::endl;
//		std::cout<<"buffer_ptr: "<<(void*)buffer_ptr<<std::endl;
//		std::cout<<"buffer_ptr1: "<<(void*)buffer_ptr1<<std::endl;
//		std::cout<<"buff stride: "<<s_pair.first.buffer_stride<<std::endl;
//		std::cout<<std::endl;

		s_pair.second->seekg(point, std::ios::beg);
		s_pair.second->read(buffer_ptr1, s_pair.first.unit_size);



		//std::cout<<buffer_ptr<<std::endl;
		convert(s_pair , buffer_ptr1);
		s_pair.first.jump();
	}


	uint32_t getMaxAttributeCount(tinygltf::Model model, tinygltf::Primitive prim){
		uint32_t max_count = 0;
		for (auto &attrib : prim.attributes) {
			max_count = std::max((uint32_t)model.accessors[attrib.second].count, max_count);
		}
		return max_count;
	}


//	void loadIndice(std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> s_pair, char* buffer_ptr){
//
//	}


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
			if (buffer_map.find(buff.uri) != buffer_map.end()) continue;
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


	std::pair<std::pair<VkBuffer,VkDeviceMemory>, uint32_t> loadVertexBuffer(
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



		for (uint32_t i_attrib = 0; i_attrib < gltfNames.size(); ++i_attrib){
			std::string attrib = gltfNames[i_attrib];
			if (prim.attributes.count(attrib) == 0) continue;

			tinygltf::Accessor accessor = model.accessors[prim.attributes[attrib]];
			tinygltf::BufferView buffView = model.bufferViews[accessor.bufferView];
			tinygltf::Buffer buff = model.buffers[buffView.buffer];


			std::any default_val{};


			if (attrib == "COLOR_0") {
				default_val = 1.0f;
			}else if (attrib == "COLOR_1") {
				default_val = 1.0f;
			}else if (attrib == "COLOR_2") {
				default_val = 1.0f;
			}else if (attrib == "COLOR_3") {
				default_val = 1.0f;
			}else{
				default_val = 0;
			}

			double norm_factor = 1.0f;



			uint32_t sz = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);
			//std::cout<<"GetComponentSizeInBytes: "<<tinygltf::GetComponentSizeInBytes(accessor.componentType)<<std::endl;
			//std::cout<<"GetNumComponentsInType: "<<tinygltf::GetNumComponentsInType(accessor.type)<<std::endl;
			//std::cout<<"sz: "<<sz<<std::endl;
			std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> p = std::make_pair(
								buffer_parse_context{
												default_val,
												norm_factor,
												sz,
												buffView.byteOffset + accessor.byteOffset,
												buffView.byteStride > 0 ? buffView.byteStride : sz,
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

		void* r_data;
		vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &r_data);

		std::memset(r_data, 0, bufferSize);
		char* data = (char*)r_data;
		//std::cout<<(void*)data<<std::endl;
		for (int i_vertex = 0; i_vertex < vert_max_count; ++i_vertex) {
			for (int i_attrib = 0; i_attrib < attrib_sources.size(); ++i_attrib) {
				loadPart(attrib_sources[i_attrib], data);
			}
		}
		//memcpy(data, vertices.data(), (size_t) bufferSize);

		vkUnmapMemory(cord->device, stagingBufferMemory);

		createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

		copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
		vkFreeMemory(cord->device, stagingBufferMemory, nullptr);

		for (int i = 0; i < attrib_sources.size(); ++i) {
			attrib_sources[i].second->close();
		}

		return std::make_pair(std::make_pair(vertexBuffer, vertexBufferMemory), vert_max_count);
   }

	std::pair<std::pair<VkBuffer,VkDeviceMemory>, uint32_t> loadIndexBuffer(
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
		default_val = (uint32_t)0;

		uint32_t sz = tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type);

		std::pair<buffer_parse_context, std::shared_ptr<std::ifstream>> indices_source = std::make_pair(
		buffer_parse_context{
//						default_val,
//						tinygltf::GetComponentSizeInBytes(accessor.componentType) * tinygltf::GetNumComponentsInType(accessor.type),
//						buffView.byteOffset + accessor.byteOffset,
//						buffView.byteStride,
						default_val,
						1,
						sz,
						buffView.byteOffset + accessor.byteOffset,
						buffView.byteStride > 0 ? buffView.byteStride : sz,
						0,
						getVulkanComponentSize(Indice::getScalarIndiceFormat()),
						accessor.type,
						accessor.componentType,
						Indice::getScalarIndiceFormat()
		},
		initialize_buffer_indices_ifstream(model, prim));


		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(cord,bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* r_data;
		vkMapMemory(cord->device, stagingBufferMemory, 0, bufferSize, 0, &r_data);
		char* data = (char*)r_data;

		for (int i_indice = 0; i_indice < ind_max_count; ++i_indice) {
			loadPart(indices_source, data);
		}
		   //memcpy(data, indices.data(), (size_t) bufferSize);
		vkUnmapMemory(cord->device, stagingBufferMemory);

		createBuffer(cord, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

		copyBuffer(cord, copy_buf, copy_queue, stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(cord->device, stagingBuffer, nullptr);
		vkFreeMemory(cord->device, stagingBufferMemory, nullptr);

		indices_source.second->close();
		return std::make_pair(std::make_pair(indexBuffer,indexBufferMemory), ind_max_count);
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
