#include <nest/pools/command_pool.h>
#include <logger/harpy_little_error.h>

using resource = harpy::nest::resources::common_vulkan_resource;
//Just for now: duplicated code

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

    VkCommandBuffer buffer{};
    
    vkAllocateCommandBuffers(*device, &secondary_ci, &buffer);
    buffer_set.secondary_buffers[buffer] = 0;
}

harpy::nest::pools::command_pool::command_pool(uint32_t family_indice, VkDevice*device) : device(device) {
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

    VkCommandBuffer buffer{};

    vkAllocateCommandBuffers(*device, &secondary_ci, &buffer);
    buffer_set.secondary_buffers[buffer] = 0;
}

harpy::nest::pools::command_pool::command_pool(command_pool && pool) {
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    buffer_set = std::move(pool.buffer_set);
    pool.buffer_set.primary_buffers.clear();
    pool.buffer_set.secondary_buffers.clear();
}

harpy::nest::pools::command_pool & harpy::nest::pools::command_pool::operator=(command_pool && pool) {
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    buffer_set = std::move(pool.buffer_set);
    pool.buffer_set.primary_buffers.clear();
    pool.buffer_set.secondary_buffers.clear();
    return *this;
}


void harpy::nest::pools::command_pool::allocate_buffers(bool is_primary, int amount) {
    if(amount <= 0)
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
        for(auto& i : buffer)
            buffer_set.secondary_buffers[i] = 0;
}

harpy::nest::resources::buffer_set_resource& harpy::nest::pools::command_pool::get_buffer_set()
{
    return buffer_set;
}

std::vector<VkCommandBuffer>& harpy::nest::pools::command_pool::get_primary_buffers()
{
    return buffer_set.primary_buffers;
}

std::unordered_map<VkCommandBuffer, std::size_t>& harpy::nest::pools::command_pool::get_secondary_buffers()
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
