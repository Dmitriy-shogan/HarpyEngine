#pragma once
#ifndef HARPY_VULKAN_WRAPPER_GRAPHICS_QUEUE
#define HARPY_VULKAN_WRAPPER_GRAPHICS_QUEUE

#include <nest/wrappers/queues/queue_family.h>

namespace harpy::nest::wrappers
{
    class graphics_family_queue : public queue_family
    {
        
    public:
        graphics_family_queue(uint32_t family) : queue_family(graphics, family, 1){}
    };
}
#endif