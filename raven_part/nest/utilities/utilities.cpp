#include "utilities.h"
#include <utilities/harpy_little_error.h>

std::vector<char> harpy::utilities::read_file(std::string filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
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

void harpy::utilities::vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDevice& device,
    VkCommandPool& pool, VkQueue transfer_queue)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = pool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(transfer_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(transfer_queue);

    vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
}

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
