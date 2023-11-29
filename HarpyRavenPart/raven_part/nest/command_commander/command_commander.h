#pragma once
#ifndef HARPY_NEST_COMMAND_COMMANDER
#define HARPY_NEST_COMMAND_COMMANDER
#include <nest/resources/command_thread_resource.h>
#include <utilities/util/delegate.h>

namespace harpy::nest
{
    class command_commander
    {
        enum res_state
        {
            null,
            started_recording,
            in_render_pass,
            out_of_render_pass,
            in_video_coding,
            out_video_coding,
            ended_recording
        };
        res_state state{null};
        utilities::delegate delegate{};
        wrappers::queue_type queue_type{};
        bool is_writing_to_primary{true};
        size_t primary_counter{}, secondary_counter{};
        resources::command_thread_resource* res{};
    
    public:

        command_commander* start_recording(resources::command_thread_resource* resource, size_t starting_buffer = 0);
        command_commander* record_to_secondary(size_t primary_buffer_index = 0);
        
        command_commander* move_up_primary();
        command_commander* move_down_primary();
        command_commander* move_up_secondary();
        command_commander* move_down_secondary();
        
        command_commander* clear_color();
        command_commander* clear_color_depth();
        command_commander* clear_attachments();
            
        command_commander* fill_buffer();
        command_commander* update_buffer();
        command_commander* copy_buffer(VkBuffer src, VkBuffer dst, size_t size);
        //command_commander* copy_buffer2();

        command_commander* copy_image();
        //command_commander* copy_image2();

        command_commander* copy_buffer_to_image();
        command_commander* copy_image_to_buffer();
            
        command_commander* blit_image();
            
        command_commander* resolve_image();

        command_commander* draw();
        command_commander* draw_indirect();
        command_commander* draw_indirect_count();
        command_commander* draw_multi_EXT();
            
        command_commander* draw_indexed();
        command_commander* draw_indirect_indexed();
        command_commander* draw_indirect_count_indexed();
        command_commander* draw_multi_EXT_indexed();

        command_commander* bind_index_buffer();
        command_commander* bind_index_buffer2();
        command_commander* bind_vertex_buffers();
            
        command_commander* set_viewport();
        command_commander* set_scissors();
        command_commander* set_line_width();
        command_commander* set_depth_bias();
        command_commander* set_depth_bounds();
        command_commander* set_stencil_compare_mask();
        command_commander* set_stencil_write_mask();
        command_commander* set_stencil_reference();
        command_commander* set_blend_constants();

        command_commander* dispatch();
        command_commander* dispatch_indirect();
        command_commander* dispatch_base();

        command_commander* next_subpass();

            
        command_commander* end_recording_secondary();
        utilities::delegate end_recording();

    private:
        VkCommandBufferInheritanceInfo inh_ci{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
            .framebuffer = VK_NULL_HANDLE,
            
            //Needs research
            .renderPass = res->render_pass->get_vk_render_pass(),
            .subpass = 0,
            .occlusionQueryEnable = VK_FALSE,
        };

        VkCommandBufferBeginInfo primary_begin_ci {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    
            //Just for now, needs research
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            };
        VkCommandBufferBeginInfo begin_ci{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    
            //Just for now, needs research
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = &inh_ci
            };

        VkBufferCopy buffer_copy_ci{
        .size = 0,
        .dstOffset = 0,
        .srcOffset = 0,
        };
    };
}


#endif //HARPY_NEST_COMMAND_COMMANDER