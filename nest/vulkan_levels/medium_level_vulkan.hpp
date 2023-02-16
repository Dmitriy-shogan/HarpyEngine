#pragma once
#ifndef HARPY_MEDIUM_VULKAN
#define HARPY_MEDIUM_VULKAN
#include "hard_level_vulkan.hpp"

namespace harpy_nest{
    
class medium_level_vulkan
{
    hard_level_vulkan* hard_vulk;

    
    
public:
    
    medium_level_vulkan(hard_level_vulkan& vulk) : hard_vulk(&vulk){}
    


    void init_all_default();
    
};
    
}

#endif