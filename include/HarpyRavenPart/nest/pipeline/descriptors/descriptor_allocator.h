#pragma once
#ifndef HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_ALLOCATOR
#define HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_ALLOCATOR
#include <nest/pools/descriptor_pool.h>

namespace harpy::nest::pipeline::descriptors
{
    class descriptor_factory;
    inline constinit uint32_t descriptor_pool_default_max_size = 5000; 
    
    class descriptor_allocator
    {
        friend descriptor_factory;
        uint32_t current_size{};
        VkDevice* device{};
         
        pools::descriptor_pool main_pool;
        
        std::set<pools::descriptor_types> types{};
        
    public:
        descriptor_allocator(std::vector<pools::pool_size_desc>& descriptions, uint32_t max_set_amount, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        descriptor_allocator(uint32_t descriptor_amount, pools::descriptor_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        descriptor_allocator(const descriptor_allocator & allocator) = delete;
        descriptor_allocator& operator= (const descriptor_allocator & allocator) = delete;

        descriptor_allocator(descriptor_allocator&& allocator) noexcept;
        descriptor_allocator& operator= (descriptor_allocator&& allocator) noexcept;

        std::vector<VkDescriptorSet> allocate_descriptor_sets(const VkDescriptorSetLayout&, uint32_t amount);
        std::vector<VkDescriptorSet> allocate_descriptor_sets(const std::vector<VkDescriptorSetLayout>&, uint32_t amount);
        
        




        ~descriptor_allocator() = default;
    };
}

#endif //HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_ALLOCATOR