#pragma once
#ifndef HARPY_NEST_MANAGER_SWAPCHAIN
#define HARPY_NEST_MANAGER_SWAPCHAIN
#include <deque>
#include <map>
#include <nest/wrappers/swapchain/swapchain.h>
#include <nest/inititalizations.h>

namespace harpy::nest::managers
{
    
    class swapchain_manager
    {
        HARPY_MAKE_SINGLETON(swapchain_manager)
        
        std::map<std::unique_ptr<wrappers::swapchain>, bool> swapchains;
    
    public:
        
        void add_swapchain();
        void add_swapchain(std::unique_ptr<wrappers::swapchain_ci>& swap_ci);

        void make_available(wrappers::swapchain& chain);
        
        wrappers::swapchain& get_swapchain();
    };
}
#endif //HARPY_NEST_MANAGER_SWAPCHAIN