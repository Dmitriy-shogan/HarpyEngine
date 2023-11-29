#include "swapchain_manager.h"
#include <nest/resources/common_vulkan_resource.h>
#include <nest/resources/surface_capabilities.h>
std::vector<harpy::nest::wrappers::swapchain> harpy::nest::managers::swapchain_manager::swapchains{};

struct harpy::nest::wrappers::swapchain_ci{
    windowing::window& win {resources::common_vulkan_resource::get_resource().get_main_window()};
    VkSurfaceKHR surface{resources::common_vulkan_resource::get_resource()};
    uint32_t family_index {resources::common_vulkan_resource::get_resource().get_main_family_queue().get_family_index()};
    std::vector<VkFormat> ideal_formats {VK_FORMAT_R8G8B8A8_SRGB, VK_FORMAT_B8G8R8A8_SRGB};
    VkDevice device = resources::common_vulkan_resource::get_resource().get_main_device();
    resources::surface_capabilities caps {resources::std_surface_capabilities};
};

void harpy::nest::managers::swapchain_manager::init()
{
    swapchains.emplace_back().init(nullptr);
}

void harpy::nest::managers::swapchain_manager::add_swapchain()
{
    swapchains.emplace_back().init(nullptr);
}

void harpy::nest::managers::swapchain_manager::add_swapchain(std::shared_ptr<wrappers::swapchain_ci> swap_ci)
{
    swapchains.emplace_back().init(swap_ci);
}


harpy::nest::managers::swapchain_manager::swapchain& harpy::nest::managers::swapchain_manager::get_swapchain(size_t index)
{
    return swapchains.at(index);
}

std::vector<harpy::nest::managers::swapchain_manager::swapchain>& harpy::nest::managers::swapchain_manager::
get_swapchains()
{
    return swapchains;
}

harpy::nest::managers::swapchain_manager::swapchain& harpy::nest::managers::swapchain_manager::get_main_swapchain()
{return swapchains[0];
}

harpy::nest::managers::swapchain_manager::operator VkSwapchainKHR_T*&()
{return swapchains[0];}

harpy::nest::managers::swapchain_manager::operator swapchain&()
{return swapchains[0];
}
