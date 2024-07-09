#include <nest/managers/swapchain_manager.h>
#include <nest/resources/common_vulkan_resource.h>

void harpy::nest::managers::swapchain_manager::add_swapchain()
{
    swapchains[std::make_unique<wrappers::swapchain>()] = true;
}

void harpy::nest::managers::swapchain_manager::add_swapchain(std::unique_ptr<wrappers::swapchain_ci>& swap_ci)
{
    swapchains[std::make_unique<wrappers::swapchain>(swap_ci.get())] = true;
}


void harpy::nest::managers::swapchain_manager::make_available(wrappers::swapchain& chain)
{
    for(auto& i : swapchains)
    {
        if(i.first->get_vk_swapchain() == chain.get_vk_swapchain())
        {
            i.second = true;
        }
    }
}

harpy::nest::wrappers::swapchain& harpy::nest::managers::swapchain_manager::get_swapchain()
{
    if(swapchains.empty())
    {
        add_swapchain();
    }
    
    for(auto& i : swapchains)
    {
        if(i.second)
        {
            i.second = false;
            return *i.first;
        }
    }

    return *swapchains.begin()->first;
    
}
