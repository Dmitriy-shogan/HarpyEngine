#include <nest/pools/descriptor_pool.h>

harpy::nest::pools::descriptor_pool::descriptor_pool(VkDevice* device)
: device(device)
{

}

harpy::nest::pools::descriptor_pool::descriptor_pool(descriptor_pool&& pool) noexcept
{
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    types = std::move(pool.types);
    current_max_size = pool.current_max_size;
}

harpy::nest::pools::descriptor_pool& harpy::nest::pools::descriptor_pool::operator=(descriptor_pool&& pool) noexcept
{
    device = pool.device;
    this->pool = pool.pool;
    pool.pool = nullptr;
    types = std::move(pool.types);
    current_max_size = pool.current_max_size;
    return *this;
}

void harpy::nest::pools::descriptor_pool::allocate_descriptors(uint32_t descriptor_amount, descriptor_types type) {
    if(pool) {
        vkDestroyDescriptorPool(*device, pool, nullptr);
    }

    VkDescriptorPoolSize size_ci{};
    size_ci.type = static_cast<VkDescriptorType>(type);
    size_ci.descriptorCount = descriptor_amount;

    VkDescriptorPoolCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

    //Just for now
    ci.maxSets = 20;

    ci.poolSizeCount = 1;
    ci.pPoolSizes = &size_ci;

    HARPY_VK_CHECK(vkCreateDescriptorPool(*device, &ci, nullptr, &pool));
}

VkDescriptorSetLayout harpy::nest::pools::descriptor_pool::get_layout(
    std::vector<VkDescriptorSetLayoutBinding> bindings) {
    VkDescriptorSetLayoutCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    ci.bindingCount = bindings.size();
    ci.pBindings = bindings.data();

    layouts.emplace_back();
    HARPY_VK_CHECK(vkCreateDescriptorSetLayout(*device, &ci, nullptr, &layouts.back()));
    return layouts.back();
}

VkDescriptorSetLayoutBinding harpy::nest::pools::descriptor_pool::get_standard_sight_binding() {
    static VkDescriptorSetLayoutBinding binding{.binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = nullptr};
    return binding;
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

harpy::nest::pools::descriptor_pool::~descriptor_pool() {
    if(pool){
        for(auto& i : layouts)
        vkDestroyDescriptorSetLayout(*device, i, nullptr);
        vkDestroyDescriptorPool(*device, pool, nullptr);
    }
}

