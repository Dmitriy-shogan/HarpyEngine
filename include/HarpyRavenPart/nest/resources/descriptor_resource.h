#pragma once
#ifndef HARPY_NEST_RESOURCES_DESCRIPTOR
#define HARPY_NEST_RESOURCES_DESCRIPTOR
#include <volk.h>
#include <vector>


namespace harpy::nest::resources
{
    struct descriptor_resource
    {
        std::vector<VkDescriptorSetLayout> layouts{};
        std::vector<VkDescriptorSet> sets{};
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
    };
}

#endif //HARPY_NEST_RESOURCES_DESCRIPTOR