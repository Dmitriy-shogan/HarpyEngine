#include "graphics_pipeline.h"
#include <nest/initializations/inititalizations.h>
#include <nest/wrappers/render_pass/render_pass.h>
#include "pipeline_cache.h"

#include <nest/resources/common_vulkan_resource.h>
using resource = harpy::nest::resources::common_vulkan_resource;

struct harpy::nest::wrappers::graphics_pipeline_options_ci{
    VkPipelineInputAssemblyStateCreateInfo input_ass{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            .primitiveRestartEnable = VK_FALSE,};
    
    VkPipelineRasterizationStateCreateInfo rasterizer{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = resource::get_resource().get_phys_features().features.depthClamp,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_BACK_BIT,
            .frontFace = VK_FRONT_FACE_CLOCKWISE,
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
    VkPipelineVertexInputStateCreateInfo vertex_inputs{};
    VkPipelineViewportStateCreateInfo viewport_state{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1,};
};

struct harpy::nest::wrappers::graphics_pipeline_ci{
    shaders::standard_shader_set modules{};
    render_pass pass{};
    pipeline_cache* cache {nullptr};
    graphics_pipeline_options_ci options{};
    
};

void harpy::nest::wrappers::graphics_pipeline::init_layout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_ci{};
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutCreateInfo ci{};
    //TODO: DESCRIPTOR SETS
    pipeline_layout_ci.setLayoutCount = 0;
    pipeline_layout_ci.pSetLayouts = nullptr;
    pipeline_layout_ci.pushConstantRangeCount = 0;
    pipeline_layout_ci.pPushConstantRanges = nullptr;

    HARPY_VK_CHECK(vkCreatePipelineLayout(resource::get_resource(), &pipeline_layout_ci, nullptr, &layout));
}

void harpy::nest::wrappers::graphics_pipeline::init(
    std::shared_ptr<graphics_pipeline_ci> create_info = nullptr,
    bool is_wireframe,
    VkDevice device)
{
    if(create_info == nullptr)
    {
        create_info = std::make_shared<graphics_pipeline_ci>();
    }
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};

    if(create_info->modules.vertex.get_shader() && create_info->modules.fragment.get_shader())
        throw utilities::error_handling::harpy_little_error("You need both vertex and fragment shaders");
    
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
        ci.module = create_info->modules.vertex.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ci.module = create_info->modules.fragment.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    if(create_info->modules.geometry.get_shader())
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        ci.module = create_info->modules.geometry.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    if(create_info->modules.compute.get_shader())
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        ci.module = create_info->modules.compute.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    if(create_info->modules.tess_control.get_shader())
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        ci.module = create_info->modules.tess_control.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    if(create_info->modules.tess_eval.get_shader())
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        ci.module = create_info->modules.tess_eval.get_shader();
        ci.pName = "main";
        shader_stage_create_infos.push_back(ci);
    }
    
    if(is_wireframe)
        create_info->options.rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
    
    create_info->options.blend_state.pAttachments = &create_info->options.blend_attachment;

    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.stageCount = static_cast<unsigned>(shader_stage_create_infos.size());
    ci.pStages = shader_stage_create_infos.data();
    ci.pVertexInputState = &create_info->options.vertex_inputs;
    ci.pInputAssemblyState = &create_info->options.input_ass;
    ci.pViewportState = &create_info->options.viewport_state;
    ci.pRasterizationState = &create_info->options.rasterizer;
    ci.pMultisampleState = &create_info->options.multisample;
    ci.pDepthStencilState = nullptr;
    ci.pColorBlendState = &create_info->options.blend_state;
    ci.pDynamicState = &create_info->options.dynamic_states;
    ci.layout = layout;
    ci.renderPass = create_info->pass;
    ci.basePipelineHandle = VK_NULL_HANDLE; 
    ci.basePipelineIndex = -1;
    
   if(create_info->cache)
       HARPY_VK_CHECK(vkCreateGraphicsPipelines(device, *create_info->cache, 1, &ci, nullptr, &pipe));

    HARPY_VK_CHECK(vkCreateGraphicsPipelines(device, nullptr, 1, &ci, nullptr, &pipe));

}

VkPipeline& harpy::nest::wrappers::graphics_pipeline::get_vk_pipeline()
{return pipe;
}

harpy::nest::wrappers::graphics_pipeline::operator VkPipeline_T*&()
{return pipe;
}

VkPipelineLayout& harpy::nest::wrappers::graphics_pipeline::get_vk_layout()
{return layout;
}

harpy::nest::wrappers::graphics_pipeline::operator VkPipelineLayout_T*&()
{return layout;
}

void harpy::nest::wrappers::graphics_pipeline::set_id(std::string id)
{ this->id =  std::move(id);
}

std::string harpy::nest::wrappers::graphics_pipeline::get_id()
{ return id;
}

harpy::nest::wrappers::graphics_pipeline::~graphics_pipeline()
{
    vkDestroyPipeline(resource::get_resource(), pipe, nullptr);
    vkDestroyPipelineLayout(resource::get_resource(), layout, nullptr);
}
