#pragma once
#ifndef HARPY_RENDER_PASS
#define HARPY_RENDER_PASS
#include <utilities/harpy_little_error.h>
#include <interfaces/interfaces.h>


namespace harpy::nest
{

    class swapchain;
    
    class render_pass : public interfaces::IStrong_component
    {
        VkRenderPass rend{nullptr};

        VkDevice& device;
        VkFormat& format;
        
    public:
        render_pass(swapchain& chain);

        void init()
        {
            VkAttachmentDescription color_attachment{};
            color_attachment.format = format;
            color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;

            color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        
            color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        
            color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference attachment_ref{};
            attachment_ref.attachment = 0;
            attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &attachment_ref;

        
            VkRenderPassCreateInfo render_pass_create_info{};
            render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            render_pass_create_info.attachmentCount = 1;
            render_pass_create_info.pAttachments = &color_attachment;
            render_pass_create_info.subpassCount = 1;
            render_pass_create_info.pSubpasses = &subpass;

            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;

            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;

            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            render_pass_create_info.dependencyCount = 1;
            render_pass_create_info.pDependencies = &dependency;

            if(vkCreateRenderPass(device, &render_pass_create_info, nullptr, &rend) != VK_SUCCESS){
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Render pass hasn't been properly initialised");
            }
        }

        VkDevice& get_vk_device() override
        {
            return device;
        }

        VkRenderPass& get_vk_render_pass(){return rend;}
        operator VkRenderPass&(){return rend;}

        

        ~render_pass() override
        {
            vkDestroyRenderPass(device, rend, nullptr);
        }
    };
}

#endif //HARPY_RENDER_PASS