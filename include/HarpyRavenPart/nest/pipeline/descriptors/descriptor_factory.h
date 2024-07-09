#pragma once
#ifndef HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_FACTORY
#define HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_FACTORY
#include <nest/pipeline/descriptors/descriptor_allocator.h>
#include <nest/resources/descriptor_resource.h>
#include <util/delegate.h>

//TODO: descriptor allocator
namespace harpy::nest::pipeline::descriptors
{
    
    enum descriptor_shader_stage
    {
        vertex = VK_SHADER_STAGE_VERTEX_BIT,
        tesselation_c = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        tesselation_e = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
        fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        compute = VK_SHADER_STAGE_COMPUTE_BIT,
        all_graphics = VK_SHADER_STAGE_ALL_GRAPHICS,
        all_stages = VK_SHADER_STAGE_ALL,
        
        he_first = vertex,
        he_is_bm = 1,
        he_sum = 8
    };
    
    class descriptor_factory
    {
        VkDevice* device{};
        descriptor_allocator allocator;
        
        VkDescriptorSetLayoutCreateInfo layout_ci{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags = 0,
            .bindingCount = 1
        };

        VkDescriptorSetAllocateInfo descriptor_ci{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO
        };

        resources::descriptor_resource local_res{};
        std::vector<VkDescriptorSetLayout> layouts{};
        utilities::delegate delegate{};
    public:

        descriptor_factory(descriptor_allocator&& allocator,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        descriptor_factory(const descriptor_factory&) = delete;
        descriptor_factory& operator=(const descriptor_factory&) = delete;

        void bind_resourсe(resources::descriptor_resource&& res);
        resources::descriptor_resource unbind_resourсe();
        resources::descriptor_resource get_resource();
        
        descriptor_factory* create_descriptor_layout(
            std::vector<VkDescriptorSetLayoutBinding>&& ci,
            VkDescriptorSetLayoutCreateFlags flags = 0);

        descriptor_factory* create_descriptor_layout(
            VkDescriptorSetLayoutBinding ci,
            VkDescriptorSetLayoutCreateFlags flags = 0);

        descriptor_factory* allocate_descriptor_set();
        descriptor_factory* allocate_descriptor_set(uint32_t amount);

        
        

        static VkDescriptorSetLayoutBinding sight_ub_binding(descriptor_shader_stage when_changes);

        ~descriptor_factory();
    };
}

#endif //HARPY_NEST_PIPELINE_DESCRIPTORS_DESCRIPTOR_FACTORY