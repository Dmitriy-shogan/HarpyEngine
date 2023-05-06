#pragma once
#ifndef HARPY_BUFFERS_COMMAND
#define HARPY_BUFFERS_COMMAND
#include <buffers/base_buffer.h>

#include "pools/command_pool.h"

namespace harpy::nest::buffers
{
    class command_buffer
    {
        VkDevice& device;
        pools::command_pool& pool;

        VkCommandBuffer buffer{nullptr};
        
    public:
        command_buffer(VkDevice& device, pools::command_pool& pool)
            : device(device), pool(pool) {}

        VkCommandBuffer& get_vk_command_buffer(){return buffer;}
        operator VkCommandBuffer&(){return buffer;}

        void init()
        {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = pool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            if (vkAllocateCommandBuffers(device, &allocInfo, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate command buffers!");
            }
        }
    };
}
#endif //HARPY_BUFFERS_COMMAND