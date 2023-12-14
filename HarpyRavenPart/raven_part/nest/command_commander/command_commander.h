#pragma once
#ifndef HARPY_NEST_COMMAND_COMMANDER
#define HARPY_NEST_COMMAND_COMMANDER
#include <nest/resources/command_thread_resource.h>
#include <nest/resources/record_cis_resource.h>
#include <util/delegate.h>

#include "nest/vulkan_threading/primitives/fence.h"

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
        std::size_t primary_counter{}, secondary_counter{};
        std::shared_ptr<resources::command_thread_resource> thread_resource{};
        resources::records_cis records_resource{};
        VkDevice* device;
        VmaAllocator* allocator;
    
    public:

        command_commander(VkDevice* device
    = &resources::common_vulkan_resource::get_resource().get_main_device(),
    VmaAllocator* allocator =
    &resources::common_vulkan_resource::get_resource().get_main_allocator());
        void bind_thread_res(std::shared_ptr<resources::command_thread_resource> res);
        void bind_records_cis(resources::records_cis cis);

        void unbind_thread_res();
        void unbind_records_cis();

        command_commander* start_recording(long starting_buffer = 0);
        command_commander* record_to_secondary(long starting_buffer = 0, long primary_buffer_index = 0);

        command_commander* begin_render_pass();
        command_commander* end_render_pass();
        
        command_commander* bind_pipeline();

        command_commander* write_to_primary();
        command_commander* write_to_secondary();
        
        command_commander* move_up_primary();
        command_commander* move_down_primary();
        command_commander* move_up_secondary();
        command_commander* move_down_secondary();
        
        
        command_commander* clear_color();
        command_commander* clear_color_depth();
        command_commander* clear_attachments();
            
        command_commander* fill_buffer();
        command_commander* update_buffer();
        command_commander* load_into_buffer(VkBuffer dst, void* data, std::size_t size);
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
            
        command_commander* draw_indexed(uint32_t indexex_to_draw,
            uint32_t instances_to_draw = 0,
            uint32_t starting_index = 0,
            uint32_t starting_instance = 0,
            uint32_t offset = 0);
        
        command_commander* draw_indirect_indexed();
        command_commander* draw_indirect_count_indexed();
        command_commander* draw_multi_EXT_indexed();

        command_commander* bind_index_buffer(VkBuffer buffer, uint32_t offset = 0);
        command_commander* bind_index_buffer2();
        
        command_commander* bind_vertex_buffers(
            std::vector<VkBuffer>& buffers,
            uint32_t thirst_binding = 0,
            VkDeviceSize* offset = nullptr);
            
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

        void submit(std::future<bool>& is_completed, threading::primitives::fence fence);

            
        command_commander* end_recording_secondary();
        utilities::delegate end_recording(bool do_clean_resources = true);
    };
}


#endif //HARPY_NEST_COMMAND_COMMANDER