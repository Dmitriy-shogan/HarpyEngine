#pragma once
#ifndef HARPY_NEST_RESOURCES_RECORD_CIS
#define HARPY_NEST_RESOURCES_RECORD_CIS
#include <memory>
#include <queue>
#include <nest/wrappers/render_pass/render_pass.h>

namespace harpy::nest::resources
{
    //WARNING!!! NON POD STRUCT
    struct record_cis_resource
    {
        VkCommandBufferBeginInfo primary_begin_ci{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    
            //Just for now, needs research
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            };
        
        std::vector<VkCommandBufferBeginInfo> secondary_begin_ci{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    
            //Just for now, needs research
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            };

        std::vector<VkSubmitInfo> submit_cis{
          .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
        };
        
        std::vector<VkCommandBufferInheritanceInfo> inh_cis{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            
            //Needs research
            .subpass = 0,
            .framebuffer = VK_NULL_HANDLE,
            .occlusionQueryEnable = VK_FALSE,
        };
        
        std::vector<VkBufferCopy> copy_cis{};
        
        record_cis_resource(wrappers::render_pass& render_pass)
        {
            inh_cis.front().renderPass = render_pass;
            secondary_begin_ci.front().pInheritanceInfo = &inh_cis.front();
        }
    };
    using records_cis = std::shared_ptr<record_cis_resource>;
    
}

#endif //HARPY_NEST_RESOURCES_RECORD_CIS