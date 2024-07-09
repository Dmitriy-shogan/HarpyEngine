#include <nest/command_commander/command_commander.h>
#include <nest/vulkan_threading/fence.h>
#include <nest/wrappers/buffers/data_buffer.h>
#include <nest/wrappers/swapchain/swapchain.h>

void harpy::nest::command_commander::init_staging_buffer(uint32_t size)
{
    staging_buffer.~data_buffer();
    staging_buffer = wrappers::data_buffer{wrappers::buffer_type::staging, size};
}

void harpy::nest::command_commander::transit_image(texturing::texture& texture, VkImageLayout old_layout, VkImageLayout new_layout)
{
    delegate.push_back([this, text = &texture, old_layout, new_layout, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {
        VkPipelineStageFlags src{}, dst{};
        VkImageMemoryBarrier ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ci.image = text->get_vk_image();
        ci.oldLayout = old_layout;
        ci.newLayout = new_layout;

        //Just for now
        ci.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ci.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        //Just for now
        ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ci.subresourceRange.baseMipLevel = 0;
        ci.subresourceRange.levelCount = 1;
        ci.subresourceRange.baseArrayLayer = 0;
        ci.subresourceRange.layerCount = 1;

        //Just for now
        //TODO: move this implementation into different function and reimplement this trash 
        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            ci.srcAccessMask = 0;
            ci.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if
        (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            ci.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            ci.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            src = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw utilities::error_handling::harpy_little_error("Unsupported layout transition while transiting image from one layer to another");
        }

        vkCmdPipelineBarrier(
            command_buffer,
            src,
            dst,
            0,
            0,
            nullptr,
            0,
            nullptr,
            1, &ci);
    });
}

harpy::nest::command_commander::command_commander(VkDevice* device,
                                                  VmaAllocator* allocator) : device(device), allocator(allocator)
{
}

void harpy::nest::command_commander::bind_thread_res(std::unique_ptr<resources::command_thread_resource> res)
{
    thread_resource = std::move(res);
}


std::unique_ptr<harpy::nest::resources::command_thread_resource>&& harpy::nest::command_commander::unbind_thread_res()
{
    return std::move(thread_resource);
}

harpy::nest::command_commander* harpy::nest::command_commander::start_recording(long starting_buffer, VkCommandBufferBeginInfo begin_ci)
{
    if(!thread_resource)
        throw utilities::error_handling::harpy_little_error("Please first bind thread resource before starting recording");
    
    delegate.clear();
    
    primary_counter = starting_buffer;
    used_buffers.insert(thread_resource->com_pool->get_primary_buffers()[primary_counter]);
    
    delegate.push_back([this, begin_ci]()
    {
        vkBeginCommandBuffer(thread_resource->com_pool->get_primary_buffers()[primary_counter],
                             &begin_ci);
    });
    
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::record_to_secondary(long starting_buffer, long primary_buffer_index, VkCommandBufferBeginInfo begin_ci)
{
    secondary_counter = starting_buffer;

    used_buffers.insert(thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer);
    VkCommandBufferInheritanceInfo info{};
    
    info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    
    begin_ci.pInheritanceInfo = &info;
   
    thread_resource->com_pool->get_secondary_buffers().back().parent_primary_buffer = primary_buffer_index;
    delegate.push_back([this, ci = begin_ci]()
    {
        vkBeginCommandBuffer(
        thread_resource->com_pool->get_secondary_buffers()[primary_counter].buffer,
        &ci
        );
    });

    is_writing_to_primary = false;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::start_render_pass(wrappers::swapchain& swapchain, uint32_t image_index)
{
    delegate.push_back([this, image_index, &swapchain, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain.get_render_pass();
        renderPassInfo.framebuffer = swapchain.get_framebuffer(image_index);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.get_extent();

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    });
    return this;
    
}

harpy::nest::command_commander* harpy::nest::command_commander::end_render_pass()
{
    delegate.push_back([this, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {

        vkCmdEndRenderPass(command_buffer);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_pipeline(pipeline::graphics_pipeline& pipeline)
{
    delegate.push_back([this, &pipeline, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {

        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get_vk_pipeline());
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::write_to_primary()
{
    is_writing_to_primary = true;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::write_to_secondary()
{
    is_writing_to_primary = false;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::move_up_primary()
{
    ++primary_counter;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::move_down_primary()
{
    --primary_counter;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::move_up_secondary()
{
    ++secondary_counter;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::move_down_secondary()
{
    --secondary_counter;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::load_into_buffer(VkBuffer dst, void* data, std::size_t size, VkBufferCopy buf_copy)
{
    /*if(is_harpy_debug && thread_resource->queueget_type() == pipeline::graphics || thread_resource->queue_manager->get_type() == pipeline::compute)
    {
        utilities::error_handling::logger::get_logger().log(utilities::error_handling::error_severity::warning, "WARNING!, you're trying to copy buffer in a graphics or compute queue. Please, use transfer only, main or universal queues");
        utilities::error_handling::logger::get_logger().show_last_log();
    }*/
    
    delegate.push_back([this, copy = &buf_copy, dst, size, data, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer] ()
    {
        if (staging_buffer.get_size() < size) init_staging_buffer(size);
        
    void* temp_data{nullptr};
    vmaMapMemory(*allocator, staging_buffer.get_vma_allocation(), &temp_data);
    std::memcpy(temp_data, data, size);
    vmaUnmapMemory(*allocator, staging_buffer.get_vma_allocation());

    copy->size = size;
        vkCmdCopyBuffer(
        command_buffer,
        staging_buffer.get_vk_buffer(),
        dst,
        1,
        copy);
    });

    
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::load_into_texture(texturing::texture& texture, wrappers::swapchain& swapchain)
{
    delegate.push_back([this, chain = &swapchain, text = &texture, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer] ()
    {
        transit_image(*text);
        if (staging_buffer.get_size() < text->get_size()) init_staging_buffer(text->get_size());
        
        void* temp_data{nullptr};
        vmaMapMemory(*allocator, staging_buffer.get_vma_allocation(), &temp_data);
        std::memcpy(temp_data, text->get_raw_cv_image_ptr()->get_cv_data().data, text->get_size());
        vmaUnmapMemory(*allocator, staging_buffer.get_vma_allocation());

        VkBufferImageCopy copy{};
        copy.bufferOffset = 0;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;

        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.mipLevel = 0;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;

        copy.imageOffset = {0, 0, 0};
        copy.imageExtent = {chain->get_extent().width, chain->get_extent().height, 1};
        
        vkCmdCopyBufferToImage(
            command_buffer,
            staging_buffer.get_vk_buffer(),
            text->get_vk_image(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &copy);

        transit_image(*text, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    });
    return this;
}


harpy::nest::command_commander* harpy::nest::command_commander::load_vertex_index_buffers(void* vertex_data,
                                                                                          size_t vertex_size,
                                                                                          const std::vector<uint32_t>& indice_data,
                                                                                          VkBuffer vertex_buffer,
                                                                                          VkBuffer index_buffer)
{
    delegate.push_back([this, vertex_data, vertex_size, indice_data, vertex_buffer, index_buffer, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer] ()
    {
        auto const indice_size = indice_data.size() * sizeof(uint32_t);
        if(indice_size + vertex_size > staging_buffer.get_size())
            init_staging_buffer(indice_size + vertex_size);

        int32_t* temp_data{nullptr};
    vmaMapMemory(*allocator, staging_buffer.get_vma_allocation(), reinterpret_cast<void**>(&temp_data));
        std::memcpy(temp_data, indice_data.data(), indice_size);
        std::memcpy(temp_data + indice_data.size(), vertex_data, vertex_size);
    vmaUnmapMemory(*allocator, staging_buffer.get_vma_allocation());

        VkBufferCopy copies[2] = {
            {
                .srcOffset = 0,
                .dstOffset = 0,
                .size = indice_size
            },
            {
                .srcOffset =  indice_size,
                .dstOffset = 0,
                .size = vertex_size
            }
        };
        
        vkCmdCopyBuffer(command_buffer, staging_buffer.get_vk_buffer(), index_buffer, 1, copies);
        vkCmdCopyBuffer(command_buffer, staging_buffer.get_vk_buffer(), vertex_buffer, 1, &copies[1]);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::draw_indexed(
    uint32_t indexes_to_draw,
    uint32_t instances_to_draw,
    uint32_t starting_index,
    uint32_t starting_instance,
    int offset
    )
{
    delegate.push_back([this, indexes_to_draw, instances_to_draw, starting_index, starting_instance, offset, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer] ()
    {
        vkCmdDrawIndexed(
        command_buffer,
        indexes_to_draw,
        instances_to_draw,
        starting_index,
        offset,
        starting_instance);
    });
    return this;
}


harpy::nest::command_commander* harpy::nest::command_commander::bind_index_buffer(VkBuffer buffer, uint32_t offset)
{
    delegate.push_back([this, buffer, offset, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {
        vkCmdBindIndexBuffer(
        command_buffer,
        buffer,
        offset,
        VK_INDEX_TYPE_UINT32);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_vertex_buffers(wrappers::data_buffer& buffer,
    VkDeviceSize* offset)
{
    delegate.push_back([this, &buffer, offset, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {
        if(!offset)
        {
            VkDeviceSize offsets[]{0};
            vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer.get_vk_buffer(), offsets);
        }
        else
            vkCmdBindVertexBuffers(command_buffer, 0, 1, &buffer.get_vk_buffer(), offset);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_vertex_buffers(std::vector<wrappers::data_buffer>& buffers, uint32_t first_binding, VkDeviceSize* offset)
{
    delegate.push_back([this, &buffers, first_binding, offset, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {
        std::vector<VkBuffer> vertex_buffers(buffers.size());
        for(size_t i = 0; auto& j : buffers)
        {
            vertex_buffers[i++] = j.get_vk_buffer();
        }
        if(!offset)
        {
            VkDeviceSize offsets[]{0};
            vkCmdBindVertexBuffers(command_buffer, first_binding, 1, vertex_buffers.data(), offsets);
        }
        else
            vkCmdBindVertexBuffers(command_buffer, first_binding, 1, vertex_buffers.data(), offset);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::set_viewport(VkViewport viewport)
{
    delegate.push_back([this, viewport, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::set_scissors(VkRect2D scissors)
{
    delegate.push_back([this, scissors, command_buffer = is_writing_to_primary ?
                                                            thread_resource->com_pool->get_primary_buffers()[primary_counter]:
                                                            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer]()
    {

        vkCmdSetScissor(command_buffer, 0, 1, &scissors);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::reset_current_com_buffer()
{
    vkResetCommandBuffer(thread_resource->com_pool->get_primary_buffers()[primary_counter], 0);
    for(auto& i : thread_resource->com_pool->get_secondary_buffers())
    {
        if (i.parent_primary_buffer == primary_counter)
            vkResetCommandBuffer(i.buffer, 0);
    }
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::reset_pool()
{
    thread_resource->com_pool->reset_pool();
    return this;
}

void harpy::nest::command_commander::submit(threading::fence fence, threading::semaphore render_finish, threading::semaphore image_acquired, bool do_wait)
{
    VkSubmitInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    //Just for now
    ci.commandBufferCount = 1;
    ci.pCommandBuffers = &thread_resource->com_pool->get_primary_buffers()[primary_counter];

    VkPipelineStageFlags wait_dst_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    ci.waitSemaphoreCount = image_acquired.get_semaphore() ? 1 : 0;
    ci.pWaitSemaphores = image_acquired.get_semaphore() ? &image_acquired.get_semaphore() : nullptr;
    ci.pWaitDstStageMask = image_acquired.get_semaphore() ? &wait_dst_mask : nullptr;

    ci.signalSemaphoreCount = 1;
    ci.pSignalSemaphores = &render_finish.get_semaphore();
    
    HARPY_VK_CHECK(vkQueueSubmit(
        thread_resource->queue,
        1,
        &ci,
        fence
        ));
    
    if(do_wait)
        HARPY_VK_CHECK(vkQueueWaitIdle(thread_resource->queue));
}

void harpy::nest::command_commander::submit_one(resources::vulkan_synchronisation_resource sync_res,
    std::vector<VkPipelineStageFlags> flags, bool do_wait)
{
    std::vector<VkSemaphore> raw_wait_semaphores{sync_res.wait_semaphores.size()},
                             raw_signal_semaphores{sync_res.signal_semaphores.size()};

    for(size_t i = 0; i < sync_res.wait_semaphores.size(); i++)
        raw_wait_semaphores[i] = sync_res.wait_semaphores[i].get_semaphore();

    for(size_t i = 0; i < sync_res.signal_semaphores.size(); i++)
        raw_signal_semaphores[i] = sync_res.signal_semaphores[i].get_semaphore();
    
    VkSubmitInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    ci.commandBufferCount = 1;
    ci.pCommandBuffers = &thread_resource->com_pool->get_primary_buffers()[primary_counter];

    ci.waitSemaphoreCount = sync_res.wait_semaphores.size();
    ci.pWaitSemaphores = raw_wait_semaphores.data();
    
    ci.pWaitDstStageMask = flags.data();

    ci.signalSemaphoreCount = sync_res.signal_semaphores.size();
    ci.pSignalSemaphores = raw_signal_semaphores.data();
    
    HARPY_VK_CHECK(vkQueueSubmit(
        thread_resource->queue,
        1,
        &ci,
        sync_res.fence
        ));
    
    if(do_wait)
        HARPY_VK_CHECK(vkQueueWaitIdle(thread_resource->queue));
}

void harpy::nest::command_commander::submit_all(resources::vulkan_synchronisation_resource sync_res, std::vector<VkPipelineStageFlags> flags, bool do_wait)
{
    std::vector<VkSemaphore> raw_wait_semaphores{sync_res.wait_semaphores.size()},
                             raw_signal_semaphores{sync_res.signal_semaphores.size()};

    for(size_t i = 0; i < sync_res.wait_semaphores.size(); i++)
        raw_wait_semaphores[i] = sync_res.wait_semaphores[i].get_semaphore();

    for(size_t i = 0; i < sync_res.signal_semaphores.size(); i++)
        raw_signal_semaphores[i] = sync_res.signal_semaphores[i].get_semaphore();
    
    VkSubmitInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkCommandBuffer> buffers{used_buffers.begin(), used_buffers.end()};
    
    ci.commandBufferCount = used_buffers.size();
    ci.pCommandBuffers = buffers.data();

    ci.waitSemaphoreCount = sync_res.wait_semaphores.size();
    ci.pWaitSemaphores = raw_wait_semaphores.data();
    
    ci.pWaitDstStageMask = flags.data();

    ci.signalSemaphoreCount = sync_res.signal_semaphores.size();
    ci.pSignalSemaphores = raw_signal_semaphores.data();
    
    HARPY_VK_CHECK(vkQueueSubmit(
        thread_resource->queue,
        1,
        &ci,
        sync_res.fence
        ));
    
    if(do_wait)
        HARPY_VK_CHECK(vkQueueWaitIdle(thread_resource->queue));
}

harpy::nest::command_commander* harpy::nest::command_commander::end_recording_secondary()
{
    delegate.push_back(vkEndCommandBuffer, thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer);
    return this;
}

harpy::utilities::delegate harpy::nest::command_commander::end_recording(bool do_clean_resources)
{
    delegate.push_back(vkEndCommandBuffer, thread_resource->com_pool->get_primary_buffers()[primary_counter]);
    try
    {
        thread_resource->submit_infos[primary_counter].pCommandBuffers = &thread_resource->com_pool->get_primary_buffers()[primary_counter];
        thread_resource->submit_infos[primary_counter].commandBufferCount = 1;
    }
    catch(std::runtime_error& err)
    {
        utilities::error_handling::logger::get_logger() << err.what();
        utilities::error_handling::logger::get_logger() << "CAREFULL. LAST OBJECT OF VKSUBMITINFO IN VECTOR WILL BE OVERWRITTEN";
        thread_resource->submit_infos.back().pCommandBuffers = &thread_resource->com_pool->get_primary_buffers()[primary_counter];
        thread_resource->submit_infos.back().commandBufferCount = 1;
    }
    if(do_clean_resources)
    {
        thread_resource.reset();
    }
    return delegate;
}
