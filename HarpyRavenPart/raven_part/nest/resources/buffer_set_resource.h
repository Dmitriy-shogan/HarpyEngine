#pragma once
#ifndef HARPY_NEST_RESOURCES_BUFFER_SET
#define HARPY_NEST_RESOURCES_BUFFER_SET
#include <nest/initializations/inititalizations.h>
#include <nest/resources/common_vulkan_resource.h>

#include "nest/wrappers/render_pass/render_pass.h"

namespace harpy::nest::resources
{
    struct second_buffers_pair
    {
        VkCommandBuffer buffer{};
        std::size_t parent_primary_buffer{};
    };
    
    struct buffer_set_resource
    {
        std::vector<VkCommandBuffer> primary_buffers{};
        std::vector<second_buffers_pair> secondary_buffers{};
    };
}


#endif //HARPY_NEST_RESOURCES_BUFFER_SET