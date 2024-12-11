#pragma once
#ifndef HARPY_NEST_COMMAND_COMMANDER
#define HARPY_NEST_COMMAND_COMMANDER

#include <set>
#include <nest/resources/command_thread_resource.h>
#include <util/delegate.h>

#include <nest/vulkan_threading/fence.h>
#include <nest/pipeline/graphics_pipeline.h>

#include "3D/uniform_buffer_objects.h"
#include <3D/mesh.h>

#include "nest/wrappers/swapchain.h"

#include "nest/resources/vulkan_synchronisation_resource.h"
#include "nest/texturing/texture.h"


namespace harpy::nest
{
    class command_commander
    {
        utilities::delegate delegate{};
        
        bool is_writing_to_primary{true};
        std::vector<VkCommandBuffer>::iterator primary_counter{};

        uint32_t secondary_counter_helper{};
        std::unordered_map<VkCommandBuffer, std::size_t>::iterator secondary_counter{};

        //Just for now
        wrappers::data_buffer staging_buffer{wrappers::buffer_type::staging};
        uint32_t staging_size{0};
        
        std::unique_ptr<resources::command_thread_resource> thread_resource{};
        std::vector<VkCommandBuffer> used_buffers{};
        
        VkDevice* device;
        VmaAllocator* allocator;

        void init_staging_buffer(uint32_t size);
        void transit_image(texturing::texture& texture,
            VkImageLayout old_layout = VK_IMAGE_LAYOUT_UNDEFINED,
            VkImageLayout new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    
    public:

        command_commander(VkDevice* device
                                = &resources::common_vulkan_resource::get_resource().get_main_device(),
                          VmaAllocator* allocator 
                                = &resources::common_vulkan_resource::get_resource().get_main_allocator());
        
        void bind_thread_res(std::unique_ptr<resources::command_thread_resource> res);
        bool is_binded();
        
        [[nodiscard]]
        std::unique_ptr<resources::command_thread_resource>&& unbind_thread_res();

        command_commander* start_recording(
            uint32_t starting_buffer = 0,
            VkCommandBufferBeginInfo begin_ci = VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            });
        
        command_commander* record_to_secondary(
            uint32_t starting_buffer = 0,
            uint32_t primary_buffer_index = 0,
            VkCommandBufferBeginInfo begin_ci = VkCommandBufferBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            });

        command_commander* start_render_pass(wrappers::swapchain& swapchain, uint32_t image_index);
        command_commander* end_render_pass();
        
        command_commander* bind_pipeline(pipeline::graphics_pipeline& pipeline);

        command_commander* write_to_primary();
        command_commander* write_to_secondary();
        
        command_commander* move_up_primary();
        command_commander* move_down_primary();
        command_commander* move_up_secondary();
        command_commander* move_down_secondary();

        command_commander* bind_descriptor_sets(VkDescriptorSet set, pipeline::graphics_pipeline& pipeline);
        command_commander* bind_descriptor_sets(std::vector<VkDescriptorSet>& sets, pipeline::graphics_pipeline& pipeline);

        command_commander* bind_push_constants(pipeline::graphics_pipeline& pipe, glm::mat4 matrix);


        command_commander* clear_color();
        command_commander* clear_color_depth();
        command_commander* clear_attachments();
            
        command_commander* fill_buffer();
        command_commander* update_buffer();

        command_commander* load_into_buffer(
            VkBuffer dst,
            void* data,
            std::size_t size);

        template<typename T>
        command_commander* load_into_buffer(
                VkBuffer dst,
                std::vector<T>& vector);

        command_commander* load_into_uniform_buffer(
            wrappers::data_buffer& buffer,
            D3::uniform_buffers::sight_ub& sight);

        command_commander* load_into_texture(texturing::texture& texture, utilities::image& image);

        command_commander* copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
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
            
        command_commander* draw_indexed(uint32_t indexes_to_draw,
            uint32_t instances_to_draw = 1,
            uint32_t starting_index = 0,
            uint32_t starting_instance = 0,
            int offset = 0);
        
        command_commander* draw_indirect_indexed();
        command_commander* draw_indirect_count_indexed();
        command_commander* draw_multi_EXT_indexed();

        command_commander* bind_index_buffer(VkBuffer buffer, uint32_t offset = 0);
        command_commander* bind_index_buffer2();

        command_commander* bind_vertex_buffers(
            wrappers::data_buffer& buffer,
            VkDeviceSize* offset = nullptr);
        
        command_commander* bind_vertex_buffers(
            std::vector<wrappers::data_buffer>& buffers,
            uint32_t first_binding = 0,
            VkDeviceSize* offset = nullptr);
            
        command_commander* set_viewport(VkViewport viewport);
        command_commander* set_scissors(VkRect2D scissors);
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

        //Just for now: Command pools don't allow to reset individual buffers
        command_commander* reset_current_com_buffer();
        command_commander* reset_pool();
        command_commander* trim_pool();


        //Just for now
        void submit(VkPipelineStageFlags wait_dst_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, threading::fence* fence = nullptr, threading::semaphore* render_finish = nullptr, threading::semaphore* image_acquired = nullptr, bool do_wait = true);
            
        command_commander* end_recording_secondary(bool do_write_to_primary = true);
        
        [[nodiscard]]
        utilities::delegate end_recording();

        void fast_load_mesh(D3::mesh& mesh, std::vector<wrappers::vertex> & vertices, std::vector<uint32_t> & indices);
        void fast_load_texture(texturing::texture& texture, utilities::image& image);
    };
}

template<typename T>
harpy::nest::command_commander *harpy::nest::command_commander::load_into_buffer(VkBuffer dst, std::vector<T>& vector) {
    delegate.push_back([this, dst, &vector, command_buffer = is_writing_to_primary ? *primary_counter : secondary_counter->first] ()
                       {
                           if (staging_buffer.get_size() < vector.size() * sizeof(vector.front()) + staging_size) init_staging_buffer(vector.size() * sizeof(vector.front()) + staging_size);

                           std::memcpy(static_cast<char*>(staging_buffer.get_mapped_ptr()) + staging_size, vector.data(), vector.size() * sizeof(vector.front()));
                          staging_buffer.unmap_ptr();

                           VkBufferCopy copy{
                                   .srcOffset = staging_size,
                                   .dstOffset = 0,
                                   .size = vector.size() * sizeof(vector.front())
                           };

                                vkCmdCopyBuffer(
                                        command_buffer,
                                        staging_buffer.get_vk_buffer(),
                                        dst,
                                        1,
                                        &copy);


                 staging_size += vector.size() * sizeof(vector.front());

             });


    return this;
}

#endif //HARPY_NEST_COMMAND_COMMANDER
