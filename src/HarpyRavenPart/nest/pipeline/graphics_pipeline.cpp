#include <nest/pipeline/graphics_pipeline.h>
using resource = harpy::nest::resources::common_vulkan_resource;


void harpy::nest::pipeline::graphics_pipeline::init_layout(std::vector<VkDescriptorSetLayout>& descriptors) {
    VkPipelineLayoutCreateInfo pipeline_layout_ci{};
    pipeline_layout_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    VkPushConstantRange range{};
    range.offset = 0;
    range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    range.size = sizeof(glm::mat4);

    VkDescriptorSetLayoutCreateInfo ci{};

    pipeline_layout_ci.setLayoutCount = descriptors.size();
    pipeline_layout_ci.pSetLayouts = descriptors.data();

    pipeline_layout_ci.pushConstantRangeCount = 1;
    pipeline_layout_ci.pPushConstantRanges = &range;

    HARPY_VK_CHECK(vkCreatePipelineLayout(resource::get_resource(), &pipeline_layout_ci, nullptr, &layout));

    layouts[layout]++;
}

harpy::nest::pipeline::graphics_pipeline::graphics_pipeline(std::unique_ptr<graphics_pipeline_ci> create_info,
    VkDevice* device) : device(device)
{
    init(std::move(create_info));
}

harpy::nest::pipeline::graphics_pipeline::graphics_pipeline(shaders::shader_set& set, graphics_pipeline const &pipe, bool is_same_layout) {
    init(set, pipe, is_same_layout);
}

harpy::nest::pipeline::graphics_pipeline::graphics_pipeline(graphics_pipeline &&other) {
    device = other.device;
    saved_ci = std::move(other.saved_ci);
    pipe = other.pipe;
    layout = other.layout;
    other.pipe = nullptr;
    other.layout = nullptr;
}

harpy::nest::pipeline::graphics_pipeline & harpy::nest::pipeline::graphics_pipeline::operator=(
    graphics_pipeline &&other) {
    device = other.device;
    saved_ci = std::move(other.saved_ci);
    pipe = other.pipe;
    layout = other.layout;
    other.pipe = nullptr;
    other.layout = nullptr;
    return *this;
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

std::vector<VkDescriptorSetLayout>& harpy::nest::pipeline::graphics_pipeline::get_descriptor_set_layouts() {
    return saved_ci->descriptor_layouts;
}


harpy::nest::pipeline::graphics_pipeline::~graphics_pipeline()
{
    if(pipe)
        vkDestroyPipeline(resource::get_resource(), pipe, nullptr);

    if(layout && !(--layouts[layout]))
        vkDestroyPipelineLayout(resource::get_resource(), layout, nullptr);
}

bool harpy::nest::pipeline::graphics_pipeline::is_children_possible() {
    return is_father;
}

harpy::nest::pipeline::graphics_pipeline harpy::nest::pipeline::graphics_pipeline::get_wireframe_child() {

    graphics_pipeline pipe_child{};
    pipe_child.device = device;
    pipe_child.layout = layout;

    auto child_ci = std::make_unique<graphics_pipeline_ci>();
    child_ci->options = saved_ci->options;
    child_ci->swapchain = saved_ci->swapchain;
    child_ci->descriptor_layouts = saved_ci->descriptor_layouts;

    child_ci->options.rasterizer.polygonMode = VK_POLYGON_MODE_LINE;

    layouts[layout]++;
    child_ci->shaders = std::move(saved_ci->shaders);
    pipe_child.init(std::move(child_ci));
    saved_ci->shaders = std::move(child_ci->shaders);

    return pipe_child;
}

void harpy::nest::pipeline::graphics_pipeline::init(std::unique_ptr<graphics_pipeline_ci> create_info) {
    init_layout(create_info->descriptor_layouts);

    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};

    if(create_info->shaders.vertex){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
        ci.module = create_info->shaders.vertex->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->shaders.fragment){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ci.module = create_info->shaders.fragment->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->shaders.geometry)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        ci.module = create_info->shaders.geometry->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->shaders.tess_control)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        ci.module = create_info->shaders.tess_control->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(create_info->shaders.tess_eval)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        ci.module = create_info->shaders.tess_eval->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }

    create_info->options.blend_state.pAttachments = &create_info->options.blend_attachment;

    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
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
    ci.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;

    HARPY_VK_CHECK(vkCreateGraphicsPipelines(
            *device,
            create_info->cache ? create_info->cache.get_vk_cache() : nullptr,
            1,
            &ci,
            nullptr,
            &pipe));

    saved_ci = std::move(create_info);

}

void harpy::nest::pipeline::graphics_pipeline::init(harpy::nest::shaders::shader_set &set,
                                                    const harpy::nest::pipeline::graphics_pipeline &pipe,
                                                    bool is_same_layout) {
    device = pipe.device;
    saved_ci = std::make_unique<graphics_pipeline_ci>();
    saved_ci->options = pipe.saved_ci->options;
    saved_ci->swapchain = pipe.saved_ci->swapchain;
    saved_ci->descriptor_layouts = pipe.saved_ci->descriptor_layouts;

    if(is_same_layout)
        init_layout(saved_ci->descriptor_layouts);
    else {
        layouts[pipe.layout]++;
        this->layout = pipe.layout;
    }
    std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};

    if(set.vertex){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
        ci.module = set.vertex->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(set.fragment){
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        ci.module = set.fragment->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(set.geometry)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        ci.module = set.geometry->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(set.tess_control)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        ci.module = set.tess_control->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }
    if(set.tess_eval)
    {
        VkPipelineShaderStageCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ci.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        ci.module = set.tess_eval->get_shader();
        ci.pName = "main";
        shader_stage_create_infos.emplace_back(ci);
    }

    saved_ci->options.blend_state.pAttachments = &saved_ci->options.blend_attachment;

    VkGraphicsPipelineCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    ci.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
    ci.stageCount = static_cast<unsigned>(shader_stage_create_infos.size());
    ci.pStages = shader_stage_create_infos.data();
    ci.pVertexInputState = &saved_ci->options.vertex_inputs;
    ci.pInputAssemblyState = &saved_ci->options.input_ass;
    ci.pViewportState = &saved_ci->options.viewport_state;
    ci.pRasterizationState = &saved_ci->options.rasterizer;
    ci.pMultisampleState = &saved_ci->options.multisample;
    ci.pDepthStencilState = nullptr;
    ci.pColorBlendState = &saved_ci->options.blend_state;
    ci.pDynamicState = &saved_ci->options.dynamic_states;
    ci.pDepthStencilState = &saved_ci->options.depth_stencil;
    ci.layout = layout;
    ci.renderPass = saved_ci->swapchain->get_render_pass();
    ci.basePipelineHandle = pipe.pipe;
    ci.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;

    HARPY_VK_CHECK(vkCreateGraphicsPipelines(
            *device,
            saved_ci->cache ? saved_ci->cache.get_vk_cache() : nullptr,
            1,
            &ci,
            nullptr,
            &this->pipe));
}

harpy::nest::pipeline::graphics_pipeline::graphics_pipeline(VkDevice *device) : device(device) {

}
