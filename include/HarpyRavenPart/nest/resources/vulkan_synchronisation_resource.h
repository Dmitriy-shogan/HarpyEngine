#pragma once
#ifndef HARPY_NEST_RESOURCES_VULKAN_SYNCHRONISATION
#define HARPY_NEST_RESOURCES_VULKAN_SYNCHRONISATION
#include <nest/vulkan_threading/fence.h>
#include <nest/vulkan_threading/semaphor.h>

namespace harpy::nest::resources
{
    struct vulkan_synchronisation_resource
    {
        std::vector<threading::semaphore> wait_semaphores{};
        std::vector<threading::semaphore> signal_semaphores{};

        threading::fence fence{};
    };
}
#endif //HARPY_NEST_RESOURCES_VULKAN_SYNCHRONISATION