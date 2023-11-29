#pragma once
#ifndef HARPY_NEST_RESOURCES_SURFACE_CAPABILITIES
#define HARPY_NEST_RESOURCES_SURFACE_CAPABILITIES
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::resources
{
    struct surface_capabilities
    {
        bool is_inited{false};
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkFormat> formats;
        std::vector<VkColorSpaceKHR> color_spaces;
        std::vector<VkPresentModeKHR> present_modes;
        VkCompositeAlphaFlagBitsKHR composite_alpha;
    };

    surface_capabilities create_surface_capabilities();

    static surface_capabilities std_surface_capabilities{};
}

#endif //HARPY_NEST_RESOURCES_SURFACE_CAPABILITIES