#pragma once
#ifndef HARPY_NEST_POOLS_DESCRIPTOR
#define HARPY_NEST_POOLS_DESCRIPTOR
#include <set>
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::pools
{
    enum class descriptor_types
    {
        uniform_buffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        storage_buffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        storage_image = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        sampled_image = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        
        

        he_first = storage_buffer,
        he_is_bm = 0,
        he_sum = 4
    };

    struct pool_size_desc
    {
        descriptor_types type{};
        uint32_t amount{};
    };
    
    class descriptor_pool
    {
        VkDevice* device{};
        VkDescriptorPool pool{};
        std::set<descriptor_types> types{};
        size_t max_descriptor_amount{};
    public:
        descriptor_pool(uint32_t descriptor_amount, descriptor_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        descriptor_pool(std::vector<pool_size_desc>& descriptions, uint32_t max_set_amount, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        descriptor_pool(descriptor_pool const& pool) = delete;
        descriptor_pool& operator=(descriptor_pool const& pool) = delete;

        descriptor_pool(descriptor_pool&& pool) noexcept;
        descriptor_pool& operator=(descriptor_pool&& pool) noexcept;

        VkDescriptorPool& get_vk_pool();
        operator VkDescriptorPool&();

        std::set<descriptor_types> get_types();
        bool check_type(descriptor_types);
        
        ~descriptor_pool();
    };
}


#endif //HARPY_NEST_POOLS_DESCRIPTOR