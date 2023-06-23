#pragma once
#ifndef HARPY_BASE_BUFFER
#define HARPY_BASE_BUFFER
#include <utilities/harpy_little_error.h>
#include <utilities/utilities.h>
#include <pools/command_pool.h>
#include "spinal_cord/vulkan_spinal_cord.h"

namespace harpy::nest::buffers
{
    //TODO: create staging buffer

    
    class base_buffer
    {
    protected:
        VkBuffer buffer{};
        VkDeviceMemory buffer_mem{};
        long buffer_size{};
        
        pools::command_pool& pool;
        vulkan_spinal_cord& vulkan_backend;
        
        virtual void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
            VkBufferCreateInfo buffer_create_info{};
            buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            buffer_create_info.size = size;
            buffer_create_info.usage = usage;
            buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(vulkan_backend.get_vk_device(), &buffer_create_info, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements mem_requirements;
            vkGetBufferMemoryRequirements(vulkan_backend.get_vk_device(), buffer, &mem_requirements);

            VkMemoryAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            alloc_info.allocationSize = mem_requirements.size;
            alloc_info.memoryTypeIndex = utilities::find_memory_types(vulkan_backend.get_vk_physical_device(), mem_requirements.memoryTypeBits, properties);

            if (vkAllocateMemory(pool.get_vk_device(), &alloc_info, nullptr, &buffer_mem) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(vulkan_backend.get_vk_device(), buffer, buffer_mem, 0);
        }

        //TODO: remove when staging buffer created
        static void create_buffer(VkDevice& device, VkPhysicalDevice& ph_device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = utilities::find_memory_types(ph_device, memRequirements.memoryTypeBits, properties);

            if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(device, buffer, bufferMemory, 0);
        }
        
        /*virtual void copy_buffer(VkBuffer& dest, VkCommandPool& pool)
        {
            VkCommandBufferAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            alloc_info.commandPool = pool;
            alloc_info.commandBufferCount = 1;

            VkCommandBuffer com_buf;
            vkAllocateCommandBuffers(vulkan_backend.get_vk_device(), &alloc_info, &com_buf);

            VkCommandBufferBeginInfo begin_info{};
            begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        
            vkBeginCommandBuffer(com_buf, &begin_info);

            VkBufferCopy copy_reg{};
            copy_reg.size = buffer_size;
        
            vkCmdCopyBuffer(com_buf, buffer, dest, 1, &copy_reg);
            vkEndCommandBuffer(com_buf);

            VkSubmitInfo submit_info{};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &com_buf;

            vkQueueSubmit(vulkan_backend.get_vk_graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
            vkQueueWaitIdle(vulkan_backend.get_vk_graphics_queue());

            vkFreeCommandBuffers(this->pool.get_vk_device(), pool, 1, &com_buf);
        }*/
        
    public:
        base_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
        : pool(pool), vulkan_backend(cord) {}

        virtual void init() = 0;

        virtual ~base_buffer()
        {
            if(buffer) vkDestroyBuffer(pool.get_vk_device(), buffer, nullptr);
            if (buffer_mem) vkFreeMemory(pool.get_vk_device(), buffer_mem, nullptr);
        }

        virtual VkBuffer& get_vk_buffer(){return buffer;}
        virtual VkDeviceMemory& get_vk_buffer_memory(){return buffer_mem;}

        virtual operator VkBuffer&(){
            return buffer;
        }
    };
}
#endif