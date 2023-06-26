#pragma once
#ifndef HARPY_QUEUE_TRANSFER
#define HARPY_QUEUE_TRANSFER
#include <queues/base_queue.h>

namespace harpy::nest::queues
{
    //TODO: make one global transfer queue, if videocard can carry this, otherwise not.
    class transfer_queue : public base_queue
    {
    public:
        transfer_queue(vulkan_spinal_cord& cord) : base_queue(cord) {}
        void init() override
        {
            queue_family_index = cord.get_queue_families().transfer_families.value();
            vkGetDeviceQueue(cord.get_vk_device(),
                queue_family_index,
                0,
                &queue);
        }
    };
}
#endif