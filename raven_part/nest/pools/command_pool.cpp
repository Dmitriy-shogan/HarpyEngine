#include "command_pool.h"

using namespace harpy::nest;
void pools::command_pool::init_draw_pool()
{
    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue->get_family_index();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(device, &create_info, nullptr, &pool) != VK_SUCCESS) {
        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init command pool!");
    }
}

void pools::command_pool::init_copy_pool()
{
    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue->get_family_index();
    create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (vkCreateCommandPool(device, &create_info, nullptr, &pool) != VK_SUCCESS) {
        throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init command pool!");
    }
}

void pools::command_pool::init(queues::transfer_queue* queue)
{
    this->queue = queue;
    switch(type)
    {
    case draw:
        {
            init_draw_pool();
            break;
        }
    case copy:
        {
            init_copy_pool();
            break;
        }
    }
}
