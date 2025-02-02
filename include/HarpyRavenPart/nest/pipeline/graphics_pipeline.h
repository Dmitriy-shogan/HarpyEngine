﻿#pragma once
#ifndef HARPY_NEST_PIPELINE
#define HARPY_NEST_PIPELINE
#include <nest/shader_works/shader_module.h>
#include <nest/pipeline/pipeline_cache.h>

#include <nest/wrappers/data_buffer.h>
#include <nest/wrappers/swapchain.h>

namespace harpy::nest::pipeline
{
    static const std::vector standard_dynamic_states{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    
    
    
    struct graphics_pipeline_options_ci {
    VkPipelineInputAssemblyStateCreateInfo input_ass{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,};
    
    VkPipelineRasterizationStateCreateInfo rasterizer{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = resources::common_vulkan_resource::get_resource().get_phys_features().features.depthClamp,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f,};
    
    VkPipelineMultisampleStateCreateInfo multisample{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = VK_FALSE,
            .minSampleShading = 1.0f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable = VK_FALSE,};
    
    VkPipelineColorBlendAttachmentState blend_attachment{
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
    
    VkPipelineColorBlendStateCreateInfo blend_state{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .logicOpEnable = VK_TRUE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1
    };
        
    VkPipelineDynamicStateCreateInfo dynamic_states{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(standard_dynamic_states.size()),
        .pDynamicStates = standard_dynamic_states.data()
    };
        
    VkPipelineVertexInputStateCreateInfo vertex_inputs{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &wrappers::vertex_vk_description,
        
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(wrappers::vertex_vk_input_attribute_descriptions.size()),
        .pVertexAttributeDescriptions = wrappers::vertex_vk_input_attribute_descriptions.data(),
    };
        
    VkPipelineViewportStateCreateInfo viewport_state{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,};

        VkPipelineDepthStencilStateCreateInfo depth_stencil{.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
                .front = {},
                .back = {},
                .minDepthBounds = 0,
                .maxDepthBounds = 1.0f,};
    };

    struct graphics_pipeline_ci
    {
        shaders::shader_set shaders{};
        pipeline_cache cache {};
        graphics_pipeline_options_ci options{};
        wrappers::swapchain* swapchain{nullptr};
        std::vector<VkDescriptorSetLayout> descriptor_layouts{};
    };
    
    class graphics_pipeline
    {
        VkPipeline pipe{};
        VkPipelineLayout layout{};
        std::unique_ptr<graphics_pipeline_ci> saved_ci{nullptr};

        bool is_father = true;
        VkDevice* device{};

        static inline std::unordered_map<VkPipelineLayout, int> layouts{};

        void init_layout(std::vector<VkDescriptorSetLayout>& descriptors);


    public:

        graphics_pipeline(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        void init(std::unique_ptr<graphics_pipeline_ci> create_info);
        void init(shaders::shader_set& set, graphics_pipeline const & pipe, bool is_same_layout = true);

        graphics_pipeline(std::unique_ptr<graphics_pipeline_ci> create_info,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        graphics_pipeline(shaders::shader_set& set, graphics_pipeline const & pipe, bool is_same_layout = true);

        graphics_pipeline(graphics_pipeline&& other);
        graphics_pipeline& operator=(graphics_pipeline&& other);
        
        VkPipeline& get_vk_pipeline();
        operator VkPipeline&();

        VkPipelineLayout& get_vk_layout();
        operator VkPipelineLayout&();

        std::vector<VkDescriptorSetLayout>& get_descriptor_set_layouts();

        graphics_pipeline get_wireframe_child();
        bool is_children_possible();

        
    ~graphics_pipeline();
    };
}
#endif //HARPY_NEST_PIPELINE