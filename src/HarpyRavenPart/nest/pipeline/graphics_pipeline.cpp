#include <nest/pipeline/graphics_pipeline.h>
using resource = harpy::nest::resources::common_vulkan_resource;


void harpy::nest::pipeline::graphics_pipeline::init_layout(std::vector<VkDescriptorSetLayout>* descriptors)
{
    VkPipelineLayoutCreateInfo pipeline_layout_ci{};
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    VkDescriptorSetLayoutCreateInfo ci{};
    
    if(descriptors){
        pipeline_layout_ci.setLayoutCount = descriptors->size();
        pipeline_layout_ci.pSetLayouts = descriptors->data();

        //Push constants is a must, should be used later
        /*pipeline_layout_ci.pushConstantRangeCount = 0;
        pipeline_layout_ci.pPushConstantRanges = nullptr;*/
    }
    else {
        pipeline_layout_ci.setLayoutCount = 0;
        pipeline_layout_ci.pSetLayouts = nullptr;
    }
    HARPY_VK_CHECK(vkCreatePipelineLayout(resource::get_resource(), &pipeline_layout_ci, nullptr, &layout));
}

harpy::nest::pipeline::graphics_pipeline::graphics_pipeline(graphics_pipeline_ci* create_info, bool is_wireframe,
    VkDevice* device) : device(device)
{
    init_layout(create_info->descriptor_layouts);
    
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};
    
    if(create_info->modules.vertex){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
        ci.module = create_info->modules.vertex->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->modules.fragment){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ci.module = create_info->modules.fragment->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->modules.geometry)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        ci.module = create_info->modules.geometry->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->modules.tess_control)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        ci.module = create_info->modules.tess_control->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->modules.tess_eval)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        ci.module = create_info->modules.tess_eval->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }

    //This is wrong, you now
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
    ci.pDepthStencilState = &create_info->options.depth_stencil;
    ci.layout = layout;
    ci.renderPass = create_info->swapchain->get_render_pass();
    ci.basePipelineHandle = VK_NULL_HANDLE; 
    ci.basePipelineIndex = -1;
    
    HARPY_VK_CHECK(vkCreateGraphicsPipelines(
           *device,
           create_info->cache ? create_info->cache->get_vk_cache() : nullptr,
           1,
           &ci,
           nullptr,
           &pipe));

}

VkPipeline& harpy::nest::pipeline::graphics_pipeline::get_vk_pipeline()
{return pipe;
}

harpy::nest::pipeline::graphics_pipeline::operator VkPipeline_T*&()
{return pipe;
}

VkPipelineLayout& harpy::nest::pipeline::graphics_pipeline::get_vk_layout()
{return layout;
}

harpy::nest::pipeline::graphics_pipeline::operator VkPipelineLayout_T*&()
{return layout;
}

void harpy::nest::pipeline::graphics_pipeline::set_id(std::string id)
{ this->id =  std::move(id);
}

std::string harpy::nest::pipeline::graphics_pipeline::get_id()
{ return id;
}

harpy::nest::pipeline::graphics_pipeline::~graphics_pipeline()
{
    vkDestroyPipeline(resource::get_resource(), pipe, nullptr);
    vkDestroyPipelineLayout(resource::get_resource(), layout, nullptr);
}
