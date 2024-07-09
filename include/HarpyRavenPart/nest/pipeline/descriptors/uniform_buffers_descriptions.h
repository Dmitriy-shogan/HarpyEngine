#pragma once
#ifndef HARPY_NEST_PIPELINE_DESCRIPTORS_UNIFORM_BUFFERS_DESCRIPTIONS
#define HARPY_NEST_PIPELINE_DESCRIPTORS_UNIFORM_BUFFERS_DESCRIPTIONS

#include <nest/resources/common_vulkan_resource.h>
#include <glm/mat4x4.hpp>

namespace harpy::nest::pipeline::descriptors
{
    struct sight_uniform_buffer
    {
        uint32_t binding{0};
        glm::mat4 model{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
    };
}

#endif //HARPY_NEST_PIPELINE_DESCRIPTORS_UNIFORM_BUFFERS_DESCRIPTIONS