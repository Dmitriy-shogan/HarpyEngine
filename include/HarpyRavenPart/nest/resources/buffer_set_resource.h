#pragma once
#ifndef HARPY_NEST_RESOURCES_BUFFER_SET
#define HARPY_NEST_RESOURCES_BUFFER_SET
#include <nest/resources/common_vulkan_resource.h>


namespace harpy::nest::resources
{

    struct buffer_set_resource
    {
        std::vector<VkCommandBuffer> primary_buffers{};
        std::unordered_map<VkCommandBuffer, std::size_t> secondary_buffers{};
    };
}


#endif //HARPY_NEST_RESOURCES_BUFFER_SET