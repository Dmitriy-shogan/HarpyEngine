#pragma once
#ifndef HARPY_NEST_RESOURCES_COMMAND_THREAD
#define HARPY_NEST_RESOURCES_COMMAND_THREAD
#include <memory>
#include <nest/wrappers/queues/queue_family.h>
#include <nest/pools/command_pool.h>

#include <nest/wrappers/render_pass/render_pass.h>

namespace harpy::nest::resources
{
    struct command_thread_resource
    {
        wrappers::queue_type queue_type{};
        std::shared_ptr<VkQueue> queue{};
        std::shared_ptr<pools::command_pool> com_pool{std::make_shared<pools::command_pool>()};
        std::shared_ptr<wrappers::render_pass> render_pass;
    };
}


#endif //HARPY_NEST_RESOURCES_COMMAND_THREAD