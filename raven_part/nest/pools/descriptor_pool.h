#pragma once
#ifndef HARPY_POOLS_DESCRIPTOR
#define HARPY_POOLS_DESCRIPTOR
#include <utilities/harpy_little_error.h>

namespace harpy::nest::pools
{
    class descriptor_pool
    {
        VkDescriptorPool pool {nullptr};

        VkDevice& device;
    public:
        descriptor_pool(VkDevice& device) : device(device){}

        void init()
        {
            VkDescriptorPoolSize pool_size{};
            pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pool_size.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = 1;
            poolInfo.pPoolSizes = &pool_size;
            poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

            if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init,
                        "failed to create descriptor pool!");
            }
        }

        VkDescriptorPool& get_vk_descriptor_pool(){return pool;}
        operator VkDescriptorPool&(){return pool;}

        
        ~descriptor_pool()
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
    };
}


#endif //HARPY_POOLS_DESCRIPTOR