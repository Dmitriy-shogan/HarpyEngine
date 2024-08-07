#pragma once
#ifndef HARPY_NEST_PIPELINE_DESCRIPTOR_FACTORY
#define HARPY_NEST_PIPELINE_DESCRIPTOR_FACTORY
#include <nest/pools/descriptor_pool.h>
#include <nest/wrappers/buffers/data_buffer.h>
#include <nest/texturing/texture_sampler.h>
#include <nest/texturing/texture.h>


namespace harpy::nest::pipeline {



    class descriptor_factory {
        pools::descriptor_pool pool;
        std::vector<VkDescriptorSetLayout> layouts{};

        std::vector<VkWriteDescriptorSet> writes{};

        VkDevice* device{nullptr};
    public:
        descriptor_factory(uint32_t descriptor_amount, pools::descriptor_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        descriptor_factory(std::initializer_list<pools::pool_size_desc> descriptions, uint32_t max_descriptors_amount, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        VkDescriptorSetLayout allocate_descriptor_set_layout(std::vector<VkDescriptorSetLayoutBinding> bindings);
        VkDescriptorSetLayout allocate_descriptor_set_layout(VkDescriptorSetLayoutBinding binding);

        std::vector<VkDescriptorSet> allocate_descriptor_sets(std::vector<VkDescriptorSetLayout> layouts);
        VkDescriptorSet allocate_descriptor_sets(VkDescriptorSetLayout layout);

        descriptor_factory* start_updating_sets();

        descriptor_factory* update_sight_ub(VkDescriptorSet& set, wrappers::data_buffer& uniform_buffer);
        descriptor_factory* update_sampler(VkDescriptorSet& set, texturing::texture& texture, texturing::texture_sampler& sampler);

        void end_updating_descriptor_sets();


        static VkDescriptorSetLayoutBinding get_image_sampler_binding(uint32_t descriptors_amount = 1);
        static VkDescriptorSetLayoutBinding get_sight_ub_binding(uint32_t descriptors_amount = 1);
        ~descriptor_factory();
    };
}

#endif //HARPY_NEST_PIPELINE_DESCRIPTOR_FACTORY
