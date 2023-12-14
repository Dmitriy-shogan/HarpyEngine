#pragma once
#ifndef HARPY_NEST_MANAGER_SWAPCHAIN
#define HARPY_NEST_MANAGER_SWAPCHAIN
#include <nest/wrappers/swapchain/swapchain.h>

#include "nest/initializations/inititalizations.h"

namespace harpy::nest::managers
{
    class swapchain_manager
    {
        HARPY_MAKE_SINGLETON(swapchain_manager);
        
        using swapchain = wrappers::swapchain;
        std::vector<swapchain> swapchains;
        
    public:

        void init();
        void add_swapchain();
        void add_swapchain(std::shared_ptr<wrappers::swapchain_ci> swap_ci);

        swapchain& get_swapchain(size_t index);
        std::vector<swapchain>& get_swapchains();
        swapchain& get_main_swapchain();
        operator VkSwapchainKHR&();
        operator swapchain&();
    };
}
#endif //HARPY_NEST_MANAGER_SWAPCHAIN