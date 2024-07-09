#include <nest/pipeline/descriptors/descriptor_allocator.h>

harpy::nest::pipeline::descriptors::descriptor_allocator::descriptor_allocator(
    std::vector<pools::pool_size_desc>& descriptions, uint32_t max_set_amount, VkDevice* device)
    : device(device), main_pool(pools::descriptor_pool(descriptions, max_set_amount, device))
{
    
}

harpy::nest::pipeline::descriptors::descriptor_allocator::descriptor_allocator(uint32_t descriptor_amount,
                                                                               pools::descriptor_types type, VkDevice* device)
        :device(device), main_pool(pools::descriptor_pool(descriptor_amount, type, device))
{}

harpy::nest::pipeline::descriptors::descriptor_allocator::descriptor_allocator(
    descriptor_allocator&& allocator) noexcept
: main_pool(std::move(allocator.main_pool)),
current_size(allocator.current_size),
device(allocator.device),
types(std::move(allocator.types))
{
}

harpy::nest::pipeline::descriptors::descriptor_allocator& harpy::nest::pipeline::descriptors::descriptor_allocator::
operator=(descriptor_allocator&& allocator) noexcept
{
    main_pool = std::move(allocator.main_pool);
    this->current_size = allocator.current_size;
    this->device = allocator.device;
    this->types = std::move(allocator.types);

    return *this;
}

std::vector<VkDescriptorSet> harpy::nest::pipeline::descriptors::descriptor_allocator::allocate_descriptor_sets(
    const VkDescriptorSetLayout& layout, uint32_t amount)
{
    std::vector<VkDescriptorSet> sets{amount};
    const std::vector<VkDescriptorSetLayout> layout_array{amount, layout};
    
    VkDescriptorSetAllocateInfo alloc_ci{};
    alloc_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_ci.descriptorPool = main_pool;
    alloc_ci.descriptorSetCount = amount;
    alloc_ci.pSetLayouts = layout_array.data();

    HARPY_VK_CHECK(vkAllocateDescriptorSets(*device, &alloc_ci, sets.data()));
    return sets;
}

std::vector<VkDescriptorSet> harpy::nest::pipeline::descriptors::descriptor_allocator::allocate_descriptor_sets(
    const std::vector<VkDescriptorSetLayout>& layouts, uint32_t amount)
{
    if(layouts.size() != amount)
        throw utilities::error_handling::harpy_little_error("Uh, you need the size of vector of layouts and amount to be same value");
    
    std::vector<VkDescriptorSet> sets{amount};
    
    VkDescriptorSetAllocateInfo alloc_ci{};
    alloc_ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_ci.descriptorPool = main_pool;
    alloc_ci.descriptorSetCount = amount;
    alloc_ci.pSetLayouts = layouts.data();

    HARPY_VK_CHECK(vkAllocateDescriptorSets(*device, &alloc_ci, sets.data()));
    return sets;
}

