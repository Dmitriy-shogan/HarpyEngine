#pragma once
#ifndef HARPY_VULKAN_WRAPPER_COMPUTE_QUEUE
#define HARPY_VULKAN_WRAPPER_COMPUTE_QUEUE

#include <nest/wrappers/queues/queue_family.h>

namespace harpy::nest::wrappers
{
    class compute_family_queue : public queue_family
    {
        
    public:
        compute_family_queue(uint32_t family, unsigned queue_count) : queue_family(compute, family, queue_count){}
        
    };
}
#endif