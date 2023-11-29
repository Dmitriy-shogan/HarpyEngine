#pragma once
#ifndef HARPY_VULKAN_WRAPPER_UNIVERSAL_QUEUE
#define HARPY_VULKAN_WRAPPER_UNIVERSAL_QUEUE

#include <nest/wrappers/queues/queue_family.h>

namespace harpy::nest::wrappers
{
    class universal_family_queue : public queue_family
    {
        
    public:
        universal_family_queue(uint32_t family) : queue_family(universal, family, 1){}
    };
}
#endif