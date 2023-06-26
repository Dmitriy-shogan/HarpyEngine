#include "base_queue.h"

VkQueue& harpy::nest::queues::base_queue::get_vk_queue()
{
    return queue;
}

uint32_t harpy::nest::queues::base_queue::get_family_index()
{return queue_family_index;}
