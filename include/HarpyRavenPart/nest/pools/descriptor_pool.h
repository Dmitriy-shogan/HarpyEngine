#pragma once
#ifndef HARPY_NEST_POOLS_DESCRIPTOR
#define HARPY_NEST_POOLS_DESCRIPTOR
#include <set>
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::pools
{
    inline const size_t max_descriptors_per_pool = 5000;
    enum class descriptor_types
    {
        uniform_buffer = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        storage_buffer = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        storage_image = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        sampled_image = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        sampler = VK_DESCRIPTOR_TYPE_SAMPLER,

        he_first = uniform_buffer,
        he_is_bm = 0,
        he_sum = 5
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
        //Needs research
        size_t max_descriptor_amount{max_descriptors_per_pool};
    public:
        descriptor_pool(uint32_t descriptor_amount, descriptor_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        descriptor_pool(std::initializer_list<pools::pool_size_desc> descriptions,
                        uint32_t max_descriptors_amount = max_descriptors_per_pool,
                        VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

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