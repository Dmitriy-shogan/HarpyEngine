#include <nest/pools/descriptor_pool.h>

harpy::nest::pools::descriptor_pool::descriptor_pool(uint32_t descriptor_amount, descriptor_types type,  VkDevice* device)
: device(device), types(std::set{type})
{
    VkDescriptorPoolSize size_ci{};
    size_ci.type = static_cast<VkDescriptorType>(type);
    size_ci.descriptorCount = descriptor_amount;

    VkDescriptorPoolCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    //Just for now
    ci.maxSets = MAX_FRAMES_IN_FLIGHT;

    ci.poolSizeCount = 1;
    ci.pPoolSizes = &size_ci;

    HARPY_VK_CHECK(vkCreateDescriptorPool(*device, &ci, nullptr, &pool));
    
}

harpy::nest::pools::descriptor_pool::descriptor_pool(
        std::initializer_list<pools::pool_size_desc> descriptions, uint32_t max_descriptors_amount, VkDevice* device)
: device(device), max_descriptor_amount(max_descriptors_amount)
{
    std::vector<VkDescriptorPoolSize> sizes{descriptions.size()};
    for(int f = 0; auto& i : sizes)
    {
        i.type = static_cast<VkDescriptorType>(descriptions.begin()[f].type);
        types.emplace(data(descriptions)[f].type);
        i.descriptorCount = descriptions.begin()[f++].amount;
    }
    

    VkDescriptorPoolCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    //Just for now
    ci.maxSets = MAX_FRAMES_IN_FLIGHT;
    ci.poolSizeCount = sizes.size();
    ci.pPoolSizes = sizes.data();

    HARPY_VK_CHECK(vkCreateDescriptorPool(*device, &ci, nullptr, &pool));
}

harpy::nest::pools::descriptor_pool::descriptor_pool(descriptor_pool&& pool) noexcept
{
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    types = std::move(pool.types);
    max_descriptor_amount = pool.max_descriptor_amount;
    
}

harpy::nest::pools::descriptor_pool& harpy::nest::pools::descriptor_pool::operator=(descriptor_pool&& pool) noexcept
{
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    types = std::move(pool.types);
    max_descriptor_amount = pool.max_descriptor_amount;
    return *this;
}

VkDescriptorPool& harpy::nest::pools::descriptor_pool::get_vk_pool()
{return pool;
}

harpy::nest::pools::descriptor_pool::operator VkDescriptorPool_T*&()
{return pool;
}

std::set<harpy::nest::pools::descriptor_types> harpy::nest::pools::descriptor_pool::get_types()
{return types;
}

bool harpy::nest::pools::descriptor_pool::check_type(descriptor_types type)
{
    return types.contains(type);
}

harpy::nest::pools::descriptor_pool::~descriptor_pool()
{
    if(pool)
        vkDestroyDescriptorPool(*device, pool, nullptr);
}

