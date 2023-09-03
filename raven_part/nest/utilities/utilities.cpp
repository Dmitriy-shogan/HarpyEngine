#include "utilities.h"
#include <utilities/harpy_little_error.h>
#include <pools/command_pool.h>
#include <memory>
#include <iostream>

std::vector<char> harpy::utilities::read_file(std::string filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    if (file.tellg() == -1) throw harpy_little_error("failed to open shader file!");
    std::cout<<file.tellg()<<std::endl;
    std::vector<char> res(file.tellg());
    file.seekg(0);
    file.read(res.data(), res.size());
    file.close();
    return res;
}

std::ostream& harpy::utilities::operator<<(std::ostream& out, nest::vertex& vertex)
{
    out << "Coordinata: " << std::to_string(vertex.position.x) + " " + std::to_string(vertex.position.y) + " " <<std::endl;
    out << "Color: " << std::to_string(vertex.color.x) + " " + std::to_string(vertex.color.y) + " " + std::to_string(vertex.color.z) << std::endl;
    return out;
}

//void harpy::utilities::vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDevice& device,
//	std::shared_ptr<nest::pools::command_pool>  pool, VkQueue transfer_queue)
//{
//    VkCommandBufferAllocateInfo allocInfo{};
//    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//    allocInfo.commandPool = (pool->get_vk_command_pool());
//    allocInfo.commandBufferCount = 1;
//
//    VkCommandBuffer commandBuffer;
//    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//    VkCommandBufferBeginInfo beginInfo{};
//    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//    vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//    VkBufferCopy copyRegion{};
//    copyRegion.size = size;
//    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);
//
//    vkEndCommandBuffer(commandBuffer);
//
//    VkSubmitInfo submitInfo{};
//    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//    submitInfo.commandBufferCount = 1;
//    submitInfo.pCommandBuffers = &commandBuffer;
//
//    vkQueueSubmit(transfer_queue, 1, &submitInfo, VK_NULL_HANDLE);
//    vkQueueWaitIdle(transfer_queue);
//
//    vkFreeCommandBuffers(device, pool->get_vk_command_pool(), 1, &commandBuffer);
//}

uint32_t harpy::utilities::find_memory_types(VkPhysicalDevice& device, uint32_t typeFilter,
                                             VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw harpy_little_error("failed to find suitable memory type!");
}


VkDeviceSize harpy::utilities::align_to(VkDeviceSize val, uint32_t bound){
	return (val + bound -1) & ~(bound-1);
}



uint32_t getChannelCountForFormat(VkFormat format) {

    switch (format) {
        case VK_FORMAT_R8_UINT:
        case VK_FORMAT_R8_SINT:
        case VK_FORMAT_R16_UINT:
        case VK_FORMAT_R16_SINT:
        case VK_FORMAT_R16_SFLOAT:
        case VK_FORMAT_R32_UINT:
        case VK_FORMAT_R32_SINT:
        case VK_FORMAT_R32_SFLOAT:
            return 1;

        case VK_FORMAT_R8G8_UINT:
		case VK_FORMAT_R8G8_SINT:
		case VK_FORMAT_R16G16_UINT:
		case VK_FORMAT_R16G16_SINT:
		case VK_FORMAT_R16G16_SFLOAT:
		case VK_FORMAT_R32G32_UINT:
		case VK_FORMAT_R32G32_SINT:
		case VK_FORMAT_R32G32_SFLOAT:
			return  2;

		case VK_FORMAT_R8G8B8_UINT:
		case VK_FORMAT_R8G8B8_SINT:
		case VK_FORMAT_R16G16B16_UINT:
		case VK_FORMAT_R16G16B16_SINT:
		case VK_FORMAT_R16G16B16_SFLOAT:
		case VK_FORMAT_R32G32B32_UINT:
		case VK_FORMAT_R32G32B32_SINT:
		case VK_FORMAT_R32G32B32_SFLOAT:
			return 3;

		case VK_FORMAT_R8G8B8A8_UINT:
		case VK_FORMAT_R8G8B8A8_SINT:
		case VK_FORMAT_R16G16B16A16_UINT:
		case VK_FORMAT_R16G16B16A16_SINT:
		case VK_FORMAT_R16G16B16A16_SFLOAT:
		case VK_FORMAT_R32G32B32A32_UINT:
		case VK_FORMAT_R32G32B32A32_SINT:
		case VK_FORMAT_R32G32B32A32_SFLOAT:
			return 4;

        default:
        	throw harpy::utilities::harpy_little_error("unstated channels count");
    }
    return -1;
}

    bool isFloat(VkFormat format){
    	switch (format) {
			case VK_FORMAT_R16_SFLOAT:
			case VK_FORMAT_R32_SFLOAT:
				return true;

			case VK_FORMAT_R16G16_SFLOAT:
			case VK_FORMAT_R32G32_SFLOAT:
				return true;

			case VK_FORMAT_R16G16B16_SFLOAT:
			case VK_FORMAT_R32G32B32_SFLOAT:
				return true;

			case VK_FORMAT_R16G16B16A16_SFLOAT:
			case VK_FORMAT_R32G32B32A32_SFLOAT:
				return true;

			default:
				return false;
		}
    	return false;
    }

    bool isUInt(VkFormat format){
		switch (format) {
			case VK_FORMAT_R8_UINT:
			case VK_FORMAT_R16_UINT:
			case VK_FORMAT_R32_UINT:
				return true;

			case VK_FORMAT_R8G8B8_UINT:
			case VK_FORMAT_R16G16B16_UINT:
			case VK_FORMAT_R32G32B32_UINT:
				return true;

			case VK_FORMAT_R8G8B8_UINT:
			case VK_FORMAT_R16G16B16_UINT:
			case VK_FORMAT_R32G32B32_UINT:
				return true;

			case VK_FORMAT_R8G8B8A8_UINT:
			case VK_FORMAT_R16G16B16A16_UINT:
			case VK_FORMAT_R32G32B32A32_UINT:
				return true;

			default:
				return false;
		}
		return false;
    }

    bool isSInt(VkFormat format){
    		switch (format) {
    			case VK_FORMAT_R8_SINT:
    			case VK_FORMAT_R16_SINT:
    			case VK_FORMAT_R32_SINT:
    				return true;

    			case VK_FORMAT_R8G8B8_SINT:
    			case VK_FORMAT_R16G16B16_SINT:
    			case VK_FORMAT_R32G32B32_SINT:
    				return true;

    			case VK_FORMAT_R8G8B8_SINT:
    			case VK_FORMAT_R16G16B16_SINT:
    			case VK_FORMAT_R32G32B32_SINT:
    				return true;

    			case VK_FORMAT_R8G8B8A8_SINT:
    			case VK_FORMAT_R16G16B16A16_SINT:
    			case VK_FORMAT_R32G32B32A32_SINT:
    				return true;

    			default:
    				return false;
    		}
    		return false;
        }

    return channelCount;
}


