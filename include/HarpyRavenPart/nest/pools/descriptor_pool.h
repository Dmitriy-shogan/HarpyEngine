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
        sampler = VK_DESCRIPTOR_TYPE_SAMPLER,
    };

    //TODO: implement four descriptor set levels 0 -- global, 1 -- perpass resources, 2 -- materials (arguable), 3 -- per object
    class descriptor_pool
    {
        VkDevice* device{};
        VkDescriptorPool pool{};
        std::set<descriptor_types> types{};
        std::vector<VkDescriptorSetLayout> layouts{};
        uint32_t current_max_size{2000};
    public:

        descriptor_pool(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        descriptor_pool(descriptor_pool const& pool) = delete;
        descriptor_pool& operator=(descriptor_pool const& pool) = delete;

        descriptor_pool(descriptor_pool&& pool) noexcept;
        descriptor_pool& operator=(descriptor_pool&& pool) noexcept;

        void allocate_descriptors(uint32_t descriptor_amount, descriptor_types type);
        VkDescriptorSetLayout get_layout(std::vector<VkDescriptorSetLayoutBinding> bindings);

        static VkDescriptorSetLayoutBinding get_standard_sight_binding();


        VkDescriptorPool& get_vk_pool();
        operator VkDescriptorPool&();

        std::set<descriptor_types> get_types();
        bool check_type(descriptor_types);
        
        ~descriptor_pool();
    };
}


#endif //HARPY_NEST_POOLS_DESCRIPTOR