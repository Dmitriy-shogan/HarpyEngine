#include <nest/pools/command_pool.h>
#include <logger/harpy_little_error.h>

using resource = harpy::nest::resources::common_vulkan_resource;

harpy::nest::pools::command_pool::command_pool(wrappers::queue_family& family_indice, VkDevice* device) : device(device)
{
    VkCommandPoolCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    ci.flags = 0;
    ci.queueFamilyIndex = family_indice;

    HARPY_VK_CHECK(vkCreateCommandPool(*device, &ci, nullptr, &pool));

    VkCommandBufferAllocateInfo primary_ci{};
    primary_ci.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    primary_ci.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    primary_ci.commandPool = pool;
    //Just for now
    primary_ci.commandBufferCount = 1;
    buffer_set.primary_buffers.resize(1);
    
    vkAllocateCommandBuffers(*device, &primary_ci, buffer_set.primary_buffers.data());

    VkCommandBufferAllocateInfo secondary_ci{};
    secondary_ci.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    secondary_ci.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    secondary_ci.commandPool = pool;
    //Just for now
    secondary_ci.commandBufferCount = 1;
    
    buffer_set.secondary_buffers.resize(1);
    std::vector<VkCommandBuffer> buffer(1);
    
    vkAllocateCommandBuffers(*device, &secondary_ci, buffer.data());
    buffer_set.secondary_buffers.front().buffer = buffer.front();
}

void harpy::nest::pools::command_pool::allocate_buffers(bool is_primary, int amount)
{
    if(!amount)
        return;

    VkCommandBufferAllocateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    if(is_primary)
        ci.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    else
        ci.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    
    ci.commandPool = pool;
    ci.commandBufferCount = amount;
    std::vector<VkCommandBuffer> buffer(amount);
    vkAllocateCommandBuffers(*device, &ci, buffer.data());

    if(is_primary)
        buffer_set.primary_buffers.insert(buffer_set.primary_buffers.end(), buffer.begin(), buffer.end());
    else
    {
        buffer_set.secondary_buffers.resize(amount);
        for(int counter = 0; auto& i : buffer_set.secondary_buffers)
        {
            i.buffer = buffer[counter++];
        }
    }
}

harpy::nest::resources::buffer_set_resource& harpy::nest::pools::command_pool::get_buffer_set()
{
    return buffer_set;
}

std::vector<VkCommandBuffer>& harpy::nest::pools::command_pool::get_primary_buffers()
{
    return buffer_set.primary_buffers;
}

std::vector<harpy::nest::resources::second_buffers_pair>& harpy::nest::pools::command_pool::get_secondary_buffers()
{
    return buffer_set.secondary_buffers;
}

void harpy::nest::pools::command_pool::reset_pool()
{
    vkResetCommandPool(*device, pool, 0);
}

void harpy::nest::pools::command_pool::trim_pool()
{
    vkTrimCommandPool(*device, pool, 0);
}

VkCommandPool& harpy::nest::pools::command_pool::get_vk_pool()
{
    return pool;
}

harpy::nest::pools::command_pool::operator VkCommandPool_T*&()
{return pool;
}

harpy::nest::pools::command_pool::~command_pool()
{
    if(pool)
        vkDestroyCommandPool(*device, pool, nullptr);
}
