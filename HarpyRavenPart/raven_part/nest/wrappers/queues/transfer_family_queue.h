#pragma once
#ifndef HARPY_VULKAN_WRAPPER_TRANSFER_QUEUE
#define HARPY_VULKAN_WRAPPER_TRANSFER_QUEUE

#include <nest/wrappers/queues/queue_family.h>

namespace harpy::nest::wrappers
{
    class transfer_family_queue : public queue_family
    {
        
    public:
        transfer_family_queue(uint32_t family, unsigned queue_count) : queue_family(transfer, family, queue_count){}
        
    };
}
#endif