#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include <utilities/initializations.h>

namespace harpy_nest {
    
class utilities
{
    utilities() = default;
public:


    static std::vector<char> read_file(std::string filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);
        std::vector<char> res(file.tellg());
        file.seekg(0);
        file.read(res.data(), res.size());
        file.close();
        return res;
    };

    static void copy_vk_buffer(VkBuffer& src, VkBuffer& dest, VkDeviceSize size, VkCommandPool& pool, VkDevice device, VkQueue graph_queue)
    {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer com_buf;
        vkAllocateCommandBuffers(device, &alloc_info, &com_buf);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
        vkBeginCommandBuffer(com_buf, &begin_info);

        VkBufferCopy copy_reg{};
        copy_reg.srcOffset = 0;
        copy_reg.dstOffset = 0;
        copy_reg.size = size;
        
        vkCmdCopyBuffer(com_buf, src, dest, 1, &copy_reg);
        vkEndCommandBuffer(com_buf);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &com_buf;

        vkQueueSubmit(graph_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(graph_queue);

        vkFreeCommandBuffers(device, pool, 1, &com_buf);
    }
};
    
}

#endif //HARPY_UTILS
