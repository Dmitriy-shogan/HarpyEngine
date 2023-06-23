#pragma once
#ifndef HARPY_COMMAND_BUFFER_CONTROLLER
#define HARPY_COMMAND_BUFFER_CONTROLLER
#include <buffers/command_buffer.h>
#include <buffers/vertex_buffer.h>
#include <buffers/index_buffer.h>
#include <render_pass/render_pass.h>

#include <pipeline/pipeline.h>
#include <swapchain/swapchain.h>

namespace harpy::nest
{
    //TODO: due to safety of recording commands into buffers, better to parallel this into threads with 1 pool and 1 buffer at least
    class command_buffer_controller
    {
        
        buffers::command_buffer* connected_buffer;
        swapchain& chain;
        pipeline* pipe;

        VkViewport dynamic_viewport;
        VkRect2D dynamic_scissors;
        
        VkClearValue clear_color{{{0.0f,0.0f,0.0f,1.0f}}};
        bool ended_last = true;
        
    public:

        command_buffer_controller(swapchain& chain, pipeline& pipe) :
        connected_buffer(nullptr),
        chain(chain),
        pipe(&pipe)
        {}

        //Free block. Free for controller means that we can use this functions even after closing buffer;
        
        void free_change_clear_color(glm::vec3 vec)
        {
            clear_color.color = {vec.r, vec.g, vec.b};
        }
        void free_change_clear_color(float r, float g, float b)
        {
            clear_color.color = {r, g, b};
        }

        void free_connect_pipeline(pipeline& pipe) 
        {
            this->pipe = &pipe;
        }

        //Auto disconnect last object, if user tries to connect new
        void connect(buffers::command_buffer& buff, int image_index)
        {
            if(!ended_last)
            {
                disconnect();
            }
            connected_buffer = &buff;
            
            VkCommandBufferBeginInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            create_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            
            VkRenderPassBeginInfo renpass_info{};
            renpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renpass_info.renderPass = chain.get_render_pass();
            renpass_info.renderArea.offset.x = 0;
            renpass_info.renderArea.offset.y = 0;
            renpass_info.renderArea.extent = chain.get_vk_extent();
            renpass_info.framebuffer = chain.get_framebuffers()[image_index];

            //connect clear values
            renpass_info.clearValueCount = 1;
            renpass_info.pClearValues = &clear_color;
            
            if(vkBeginCommandBuffer(*connected_buffer, &create_info) != VK_SUCCESS)
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "Can't start writing command buffer!");

            vkCmdBeginRenderPass(*connected_buffer, &renpass_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(*connected_buffer,  VK_PIPELINE_BIND_POINT_GRAPHICS, *pipe);


            //TODO: move viewport and scissor construction in counstructor
            dynamic_viewport.x = 0.0f;
            dynamic_viewport.y = 0.0f;
            dynamic_viewport.width = static_cast<float>(chain.get_vk_extent().width);
            dynamic_viewport.height = static_cast<float>(chain.get_vk_extent().height);
            dynamic_viewport.minDepth = 0.0f;
            dynamic_viewport.maxDepth = 1.0f;
            vkCmdSetViewport(*connected_buffer, 0, 1, &dynamic_viewport);
            
            dynamic_scissors.offset = {0, 0};
            dynamic_scissors.extent = chain.get_vk_extent();
            vkCmdSetScissor(*connected_buffer, 0, 1, &dynamic_scissors);

            ended_last = false;
        }
        
        
        void draw(buffers::vertex_buffer& buff, buffers::index_buffer& index, descriptor& descript, int frame)
        {
            
            VkDeviceSize offsets[] = {0};
            VkBuffer vertexBuffers[] = {buff.get_vk_buffer()};
            vkCmdBindVertexBuffers(*connected_buffer,
                0,
                1,
                vertexBuffers,
                offsets);

            vkCmdBindIndexBuffer(*connected_buffer,
                index.get_vk_buffer(),
                0,
                VK_INDEX_TYPE_UINT32);
            
            vkCmdBindDescriptorSets(*connected_buffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipe->get_vk_pipeline_layout(),
                0,
                1,
                &descript.get_vk_descriptor_set()[frame],
                0, nullptr);

            vkCmdDrawIndexed(*connected_buffer,
                index.get_indices().size(),
                1,
                0,
                0,
                0);
        }

        void disconnect()
        {
            vkCmdEndRenderPass(*connected_buffer);
            vkEndCommandBuffer(*connected_buffer);
            ended_last = true;
        }
    
    };
}

#endif //HARPY_COMMAND_BUFFER_CONTROLLER