#pragma once
#ifndef HARPY_PIPELINE
#define HARPY_PIPELINE
#include <utilities/harpy_little_error.h>
#include <shaders/base_shader.h>
#include <render_pass/render_pass.h>

namespace harpy::nest
{
    class pipeline
    {
        render_pass& rend;
        
        VkPipeline pipe{nullptr};
        VkPipelineLayout layout{nullptr};
        
    public:

        pipeline(render_pass& rend) : rend(rend){}
        
        void init()
        {

            shaders::shader_module vertex{rend.get_vk_device()};
            shaders::shader_module fragment{rend.get_vk_device()};
            vertex.init(SHADER_PATH_BASE_VERTEX);
            fragment.init(SHADER_PATH_BASE_FRAGMENT);

            VkPipelineShaderStageCreateInfo create_vertex_info{};
            create_vertex_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            create_vertex_info.stage = VK_SHADER_STAGE_VERTEX_BIT;

            create_vertex_info.module = vertex;
            create_vertex_info.pName = "main";

            VkPipelineShaderStageCreateInfo create_fragment_info{};
            create_fragment_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            create_fragment_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            create_fragment_info.module = fragment;
            create_fragment_info.pName = "main";

            VkPipelineShaderStageCreateInfo shaderStages[] = {create_vertex_info, create_fragment_info};

            std::vector<VkDynamicState> dynamicStates = {
                VK_DYNAMIC_STATE_VIEWPORT,
                VK_DYNAMIC_STATE_SCISSOR
            };

            VkPipelineDynamicStateCreateInfo dynamicState{};
            dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
            dynamicState.pDynamicStates = dynamicStates.data();

            VkPipelineVertexInputStateCreateInfo vertex_input_info{};
            vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertex_input_info.vertexBindingDescriptionCount = 0;
            vertex_input_info.pVertexBindingDescriptions = nullptr; // Optional
            vertex_input_info.vertexAttributeDescriptionCount = 0;
            vertex_input_info.pVertexAttributeDescriptions = nullptr; // Optional

            auto bindingDescription = get_binding_description();
            auto attributeDescriptions = get_attributes_descriptions();

            vertex_input_info.vertexBindingDescriptionCount = 1;
            vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertex_input_info.pVertexBindingDescriptions = &bindingDescription;
            vertex_input_info.pVertexAttributeDescriptions = attributeDescriptions.data();
    
            VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{};
            input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

            VkPipelineViewportStateCreateInfo viewport_state{};
            viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewport_state.viewportCount = 1;
            viewport_state.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading = 1.0f; // Optional
            multisampling.pSampleMask = nullptr; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable = VK_FALSE; // Optional

            VkPipelineColorBlendAttachmentState color_blend_attachment{};
            color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            color_blend_attachment.blendEnable = VK_FALSE;
            color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
            color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

            VkPipelineColorBlendStateCreateInfo color_blending{};
            color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            color_blending.logicOpEnable = VK_FALSE;
            color_blending.logicOp = VK_LOGIC_OP_COPY; // Optional
            color_blending.attachmentCount = 1;
            color_blending.pAttachments = &color_blend_attachment;
            color_blending.blendConstants[0] = 0.0f; // Optional
            color_blending.blendConstants[1] = 0.0f; // Optional
            color_blending.blendConstants[2] = 0.0f; // Optional
            color_blending.blendConstants[3] = 0.0f; // Optional
    
            VkPipelineLayoutCreateInfo pipeline_layout_info{};
            pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipeline_layout_info.setLayoutCount = 0; // Optional
            pipeline_layout_info.pSetLayouts = nullptr; // Optional
            pipeline_layout_info.pushConstantRangeCount = 0; // Optional
            pipeline_layout_info.pPushConstantRanges = nullptr; // Optional

            if (vkCreatePipelineLayout(rend.get_vk_device(), &pipeline_layout_info, nullptr, &layout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }

            VkGraphicsPipelineCreateInfo pipelineInfo{};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = 2;
            pipelineInfo.pStages = shaderStages;

            pipelineInfo.pVertexInputState = &vertex_input_info;
            pipelineInfo.pInputAssemblyState = &input_assembly_state_create_info;
            pipelineInfo.pViewportState = &viewport_state;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = nullptr; // Optional
            pipelineInfo.pColorBlendState = &color_blending;
            pipelineInfo.pDynamicState = &dynamicState; // Optional
            pipelineInfo.layout = layout;
            pipelineInfo.renderPass = rend;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex = -1; // Optional

            if (vkCreateGraphicsPipelines(rend.get_vk_device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipe) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics pipeline!");
            }
        }

        VkPipeline& get_vk_pipeline(){return pipe;}
        VkPipelineLayout& get_vk_pipeline_layout(){return layout;}

        operator VkPipeline&(){return pipe;}

        ~pipeline()
        {
            vkDestroyPipelineLayout(rend.get_vk_device(), layout, nullptr);
            vkDestroyPipeline(rend.get_vk_device(), pipe, nullptr);
        }
    };
}
#endif //HARPY_PIPELINE