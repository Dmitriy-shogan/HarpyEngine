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
            
        }

        
        ~descriptor_pool()
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
    };
}


#endif //HARPY_POOLS_DESCRIPTOR