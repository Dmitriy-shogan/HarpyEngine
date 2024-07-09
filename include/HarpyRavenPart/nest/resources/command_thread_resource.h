#pragma once
#ifndef HARPY_NEST_RESOURCES_COMMAND_THREAD
#define HARPY_NEST_RESOURCES_COMMAND_THREAD
#include <memory>
#include <nest/wrappers/queues/queue_family.h>
#include <nest/pools/command_pool.h>

#include "nest/managers/queue_manager.h"


namespace harpy::nest::resources
{
    struct command_thread_resource
    {
        managers::queue_wrapper queue;
        std::unique_ptr<pools::command_pool> com_pool{};
        std::vector<VkSubmitInfo> submit_infos{
            {
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                      .commandBufferCount = 1,
                }}};
    };
}


#endif //HARPY_NEST_RESOURCES_COMMAND_THREAD