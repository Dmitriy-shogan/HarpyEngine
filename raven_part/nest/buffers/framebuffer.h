#pragma once
#ifndef HARPY_BUFFERS_FRAMEBUFFER
#define HARPY_BUFFERS_FRAMEBUFFER
#include <buffers/base_buffer.h>
#include <render_pass/render_pass.h>

namespace harpy::nest::buffers
{
    class framebuffer
    {
        render_pass& rend;
        
        VkFramebuffer buffer{nullptr};
    public:

        framebuffer(render_pass& rend) : rend(rend) {}

        void init(VkExtent2D& extent, VkImageView& image_view)
        {
            VkFramebufferCreateInfo framebuffer_info{};
            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = rend;
            framebuffer_info.attachmentCount = 1;
            framebuffer_info.width = extent.width;
            framebuffer_info.height = extent.height;
            framebuffer_info.layers = 1;

            
                framebuffer_info.pAttachments = &image_view;
    
                if (vkCreateFramebuffer(rend.get_vk_device(), &framebuffer_info, nullptr, &buffer) != VK_SUCCESS) {
                    throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init framebuffer!");
                }
            }
        VkFramebuffer& get_vk_framebuffer(){return buffer;}
        operator VkFramebuffer&(){return buffer;}
    
    };
}

#endif //HARPY_BUFFERS_FRAMEBUFFER