#pragma once
#ifndef HARPY_QUEUE_PRESENT
#define HARPY_QUEUE_PRESENT
#include <queues/base_queue.h>

namespace harpy::nest::queues
{
    class present_queue : public base_queue
    {
    public:
        present_queue(vulkan_spinal_cord& cord) : base_queue(cord)
        {}
        void init() override
        {
            queue_family_index = cord.get_queue_families().present_families.value();
            vkGetDeviceQueue(cord.get_vk_device(),
                queue_family_index,
                0,
                &queue);
        }
    };
}
#endif