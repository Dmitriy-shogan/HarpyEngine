#pragma once
#ifndef HARPY_SOFT_VULKAN
#define HARPY_SOFT_VULKAN
#include <vulkan_levels/medium_level_vulkan.hpp>
#include <utilities/utilities.hpp>
#include <shaders/base_shader.hpp>

namespace harpy_nest{

class soft_level_vulkan : public medium_level_vulkan
{

    VkPipelineLayout pipeline_layout{};
    VkRenderPass render_pass{};
    
    void init_graphics_pipeline();
    void init_render_pass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = surface_format.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;


        
        VkRenderPassCreateInfo render_pass_create_info;

        if(vkCreateRenderPass(device, &render_pass_create_info, nullptr, &render_pass) != VK_SUCCESS){
            throw std::runtime_error("Couldn't create render pass, line is: " + std::to_string(__LINE__));
        }
    }
public:

    soft_level_vulkan() : medium_level_vulkan(){}
    

    ~soft_level_vulkan()
    {
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    }
    
};

}



#endif // HARPY_SOFT_VULKAN