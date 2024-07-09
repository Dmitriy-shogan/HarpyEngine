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
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
        VkCompositeAlphaFlagsKHR composite_alpha;
    };

    inline surface_capabilities create_surface_capabilities(VkSurfaceKHR surface = common_vulkan_resource::get_resource().get_main_surface())
    {
        surface_capabilities caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            common_vulkan_resource::get_resource().get_phys_device(),
            surface,
            &caps.capabilities);

        uint32_t formats_amount{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            common_vulkan_resource::get_resource().get_phys_device(),
            surface,
            &formats_amount, nullptr);
        
        caps.formats.resize(formats_amount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(
            common_vulkan_resource::get_resource().get_phys_device(),
            surface,
            &formats_amount, caps.formats.data());

        uint32_t present_amount{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(
        common_vulkan_resource::get_resource().get_phys_device(),
        surface,
        &present_amount, nullptr);
        
        caps.present_modes.resize(present_amount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            common_vulkan_resource::get_resource().get_phys_device(),
            surface,
            &present_amount, caps.present_modes.data());

        caps.composite_alpha = caps.capabilities.supportedCompositeAlpha;
        
        return caps;
    }

    inline surface_capabilities std_surface_capabilities{};
}

#endif //HARPY_NEST_RESOURCES_SURFACE_CAPABILITIES