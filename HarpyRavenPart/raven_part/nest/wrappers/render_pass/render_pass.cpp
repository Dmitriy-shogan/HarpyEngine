#include "render_pass.h"
#include <nest/initializations/inititalizations.h>
#include <utilities/logger/logger.h>
#include <nest/managers/swapchain_manager.h>
#include <nest/resources/common_vulkan_resource.h>
using resource = harpy::nest::resources::common_vulkan_resource;



void harpy::nest::wrappers::render_pass::init()
{
    managers::swapchain_manager man;
    VkAttachmentDescription2 colorAttachment{};
    colorAttachment.format = man.get_main_swapchain().get_format();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    
    VkAttachmentReference2 colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription2 subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    
    VkRenderPassCreateInfo2 ci{};
    ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
    ci.attachmentCount = 1;
    ci.pAttachments = &colorAttachment;
    ci.subpassCount = 1;
    ci.pSubpasses = &subpass;


    HARPY_VK_CHECK(vkCreateRenderPass2(resource::get_resource(), &ci, nullptr, &pass));
}

VkRenderPass& harpy::nest::wrappers::render_pass::get_vk_render_pass()
{return pass;}

harpy::nest::wrappers::render_pass::operator VkRenderPass_T*&()
{return pass;}

harpy::nest::wrappers::render_pass::~render_pass()
{
    if(pass)
        vkDestroyRenderPass(resource::get_resource(), pass, nullptr);
}
