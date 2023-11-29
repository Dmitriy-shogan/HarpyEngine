#pragma once
#ifndef HARPY_VULKAN_WRAPPER_MAIN_QUEUE
#define HARPY_VULKAN_WRAPPER_MAIN_QUEUE

#include <nest/wrappers/queues/queue_family.h>

namespace harpy::nest::wrappers
{
    class main_family_queue : public queue_family
    {
        
    public:
        main_family_queue(uint32_t family) : queue_family(main, family, 1){}
    };
}
#endif