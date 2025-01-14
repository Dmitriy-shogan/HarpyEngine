﻿#include "swapchain_manager.h"
#include <nest/resources/common_vulkan_resource.h>

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
    swapchains.emplace_back().init(std::move(swap_ci));
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
