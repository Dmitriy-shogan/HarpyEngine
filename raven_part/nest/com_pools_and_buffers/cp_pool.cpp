#include "cp_pool.h"
#include <vulkan_levels/soft_level_vulkan.h>


bool harpy_nest::cp_pool::is_one = false;

harpy_nest::cp_pool::cp_pool(soft_level_vulkan* vulkan_back): device(&vulkan_back->get_device())
{
    if (is_one) throw harpy_little_error("Man, no more than ONE command pool per program. Everything you need from command pool is already here");
    queue_families.graphics_families = vulkan_back->find_queue_families().graphics_families;
    queue_families.present_families = vulkan_back->find_queue_families().present_families;
    
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_families.graphics_families.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(*device, &pool_info, nullptr, &draw_pools.at(0)) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "failed to init command pool!");
    }
    
    pool_info.queueFamilyIndex = queue_families.graphics_families.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    if (vkCreateCommandPool(*device, &pool_info, nullptr, &copy_pools.at(0)) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "failed to init command pool!");
    }
}

void harpy_nest::cp_pool::generate_copy_pools(int how_many)
{
    while (how_many)
    {
        create_copy_pool();
        how_many--;
    }
}

void harpy_nest::cp_pool::generate_draw_pools(int how_many)
{
    while (how_many)
{
    create_draw_pool();
    how_many--;
}
}

VkCommandPool& harpy_nest::cp_pool::get_copy_pool(int index)
{
    return copy_pools.at(index);
}

VkCommandPool& harpy_nest::cp_pool::get_first_copy_pool()
{return copy_pools.front();}

VkCommandPool& harpy_nest::cp_pool::get_last_copy_pool()
{return copy_pools.back();}

VkCommandPool& harpy_nest::cp_pool::get_draw_pool(int index)
{
    return draw_pools.at(index);
}

VkCommandPool& harpy_nest::cp_pool::get_first_draw_pool()
{return draw_pools.front();}

VkCommandPool& harpy_nest::cp_pool::get_last_draw_pool()
{return draw_pools.back();}

harpy_nest::cp_pool::~cp_pool()
{
    std::for_each(draw_pools.begin(), draw_pools.end(), [this](auto& pool){vkDestroyCommandPool(*device, pool, nullptr);});
    std::for_each(copy_pools.begin(), copy_pools.end(), [this](auto& pool){vkDestroyCommandPool(*device, pool, nullptr);});
}


VkCommandPool& harpy_nest::cp_pool::create_copy_pool()
{
    pool_info.queueFamilyIndex = queue_families.graphics_families.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    copy_pools.emplace_back();
    if (vkCreateCommandPool(*device, &pool_info, nullptr, &copy_pools.back()) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "failed to add copy command pool!");
    }
    return copy_pools.back();
}

VkCommandPool& harpy_nest::cp_pool::create_draw_pool()
{
    pool_info.queueFamilyIndex = queue_families.graphics_families.value();
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    draw_pools.emplace_back();
    if (vkCreateCommandPool(*device, &pool_info, nullptr, &draw_pools.back()) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "failed to add draw command pool!");
    }
    return draw_pools.back();
}
