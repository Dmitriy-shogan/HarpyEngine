#include <nest/command_commander/command_commander.h>
#include <nest/vulkan_threading/fence.h>
#include "nest/wrappers/data_buffer.h"
#include "nest/wrappers/swapchain.h"

//TODO: replace taking command buffers [] with .at()
void harpy::nest::command_commander::init_staging_buffer(uint32_t size)
{
    if(staging_buffer.get_size()) {

        auto ptr = staging_buffer.get_mapped_ptr();

        void *temp_data = new char[staging_size];
        std::memcpy(temp_data, ptr, staging_size);


        staging_buffer.unmap_ptr();
        staging_buffer.~data_buffer();

        staging_buffer = std::move(wrappers::data_buffer{wrappers::buffer_type::staging, size});

        ptr = staging_buffer.get_mapped_ptr();
        std::memcpy(ptr, temp_data, staging_size);
        staging_buffer.unmap_ptr();

        delete[] static_cast<char*>(temp_data);
    } else
        staging_buffer.init(size);
}


void harpy::nest::command_commander::transit_image(texturing::texture& texture, VkImageLayout old_layout, VkImageLayout new_layout)
{
        auto command_buffer = is_writing_to_primary ? *primary_counter : secondary_counter->first;
        VkPipelineStageFlags src{}, dst{};
        VkImageMemoryBarrier ci{};
        ci.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ci.image = texture.get_vk_image();
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
            throw utilities::harpy_little_error("Unsupported layout transition while transiting image from one layer to another");
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
}

harpy::nest::command_commander::command_commander(VkDevice* device,
                                                  VmaAllocator* allocator) : device(device), allocator(allocator)
{
}

void harpy::nest::command_commander::bind_thread_res(std::unique_ptr<resources::command_thread_resource> res)
{
    thread_resource = std::move(res);
}

bool harpy::nest::command_commander::is_binded() {
    return thread_resource.get();
}


std::unique_ptr<harpy::nest::resources::command_thread_resource>&& harpy::nest::command_commander::unbind_thread_res()
{
    used_buffers.clear();
    is_writing_to_primary = true;
    delegate.clear();
    return std::move(thread_resource);
}

harpy::nest::command_commander* harpy::nest::command_commander::start_recording(uint32_t starting_buffer, VkCommandBufferBeginInfo begin_ci)
{
    if(!thread_resource)
        throw utilities::harpy_little_error("Please first bind thread resource before starting recording");
    
    delegate.clear();
    
    primary_counter = thread_resource->com_pool->get_primary_buffers().begin();
    primary_counter += starting_buffer;

    used_buffers.push_back(*primary_counter);
    thread_resource->submit_infos.commandBufferCount = 1;
    
    delegate.push_back([this, begin_ci]()
    {
        vkBeginCommandBuffer(*primary_counter,
                             &begin_ci);
    });
    
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::record_to_secondary(uint32_t starting_buffer, uint32_t primary_buffer_index, VkCommandBufferBeginInfo begin_ci)
{
    secondary_counter = thread_resource->com_pool->get_secondary_buffers().begin();
    while(starting_buffer-- > 0) {
        ++secondary_counter;
    }
    secondary_counter->second = primary_buffer_index;
    delegate.push_back([this, ci = begin_ci] () mutable
    {
        VkCommandBufferInheritanceInfo info{};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
        ci.pInheritanceInfo = &info;
        vkBeginCommandBuffer(
        secondary_counter->first,
        &ci
        );
    });

    is_writing_to_primary = false;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::start_render_pass(wrappers::swapchain& swapchain, uint32_t image_index)
{
    delegate.push_back([this, image_index, &swapchain, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapchain.get_render_pass();
        renderPassInfo.framebuffer = swapchain.get_framebuffer(image_index);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapchain.get_extent();

        std::vector<VkClearValue> clearColor{2};
        clearColor[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearColor[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = clearColor.size();
        renderPassInfo.pClearValues = clearColor.data();


        vkCmdBeginRenderPass(command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    });
    return this;
    
}

harpy::nest::command_commander* harpy::nest::command_commander::end_render_pass()
{
    delegate.push_back([this, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]()
    {

        vkCmdEndRenderPass(command_buffer);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_pipeline(pipeline::graphics_pipeline& pipeline)
{
    delegate.push_back([this, &pipeline, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]()
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
    secondary_counter_helper++;
    ++secondary_counter;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::move_down_secondary()
{
    secondary_counter = thread_resource->com_pool->get_secondary_buffers().begin();
    for(auto i  = 0; i < --secondary_counter_helper; i++)
        secondary_counter++;
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::load_into_buffer(VkBuffer dst, void* data, std::size_t size)
{
    /*if(is_harpy_debug && thread_resource->queueget_type() == pipeline::graphics || thread_resource->queue_manager->get_type() == pipeline::compute)
    {
        utilities::logger::get_logger().log(utilities::error_severity::warning, "WARNING!, you're trying to copy buffer in a graphics or compute queue. Please, use transfer only, main or universal queues");
        utilities::logger::get_logger().show_last_log();
    }*/
    
    delegate.push_back([this, dst, size, data, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first] ()
    {
        if (staging_buffer.get_size() < size + staging_size) init_staging_buffer(size + staging_size);

    void* temp_data{nullptr};
    vmaMapMemory(*allocator, staging_buffer.get_vma_allocation(), &temp_data);
    std::memcpy(static_cast<char*>(temp_data) + staging_size, data, size);
    vmaUnmapMemory(*allocator, staging_buffer.get_vma_allocation());

    VkBufferCopy copy{
        .srcOffset = staging_size,
        .dstOffset = 0,
        .size = size
    };

        vkCmdCopyBuffer(
        command_buffer,
        staging_buffer.get_vk_buffer(),
        dst,
        1,
        &copy);
        staging_size += size;

    });

    
    return this;
}

harpy::nest::command_commander * harpy::nest::command_commander::load_into_uniform_buffer(wrappers::data_buffer &buffer,
    D3::uniform_buffers::sight_ub &sight) {
    delegate.push_back([this, &buffer, sight] () mutable {
                                                                if(buffer.get_type() != wrappers::buffer_type::uniform)
                                                                    throw utilities::harpy_little_error("Sorry, we don't allow to load into non uniform buffers in LOAD INTO UNIFORM BUFFERS");
        auto ptr = buffer.get_mapped_ptr();
        std::memcpy(ptr, &sight, sizeof(D3::uniform_buffers::sight_ub));
        buffer.unmap_ptr();
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::load_into_texture(texturing::texture& texture, utilities::image& image)
{
    delegate.push_back([this, image = &image, text = &texture, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first] ()
    {
        transit_image(*text);
        if (staging_buffer.get_size() < text->get_size() + staging_size) init_staging_buffer(staging_size + text->get_size());

        auto ptr = staging_buffer.get_mapped_ptr();
        std::memcpy(static_cast<char*>(ptr) + staging_size, image->get_cv_data().data, text->get_size());
        staging_buffer.unmap_ptr();

        VkBufferImageCopy copy{};
        copy.bufferOffset = staging_size;
        copy.bufferRowLength = 0;
        copy.bufferImageHeight = 0;

        copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copy.imageSubresource.mipLevel = 0;
        copy.imageSubresource.baseArrayLayer = 0;
        copy.imageSubresource.layerCount = 1;

        copy.imageOffset = {0, 0, 0};
        copy.imageExtent = {image->dimension_sizes().first, image->dimension_sizes().second, 1};

        staging_size += text->get_size();
        
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

harpy::nest::command_commander * harpy::nest::command_commander::copy_buffer(VkBuffer src, VkBuffer dst,
    VkDeviceSize size) {
    delegate.push_back([this, src, dst, size, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first] () {
        VkBufferCopy copy{};
        copy.srcOffset = 0;
        copy.dstOffset = 0;
        copy.size = size;
        vkCmdCopyBuffer(command_buffer, src, dst, 1, &copy);
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
                                                            *primary_counter:
                                                            secondary_counter->first] ()
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
                                                            *primary_counter:
                                                            secondary_counter->first]()
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
                                                            *primary_counter:
                                                            secondary_counter->first]()
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
                                                            *primary_counter:
                                                            secondary_counter->first]()
    {
        std::vector<VkBuffer> vertex_buffers(buffers.size());
        for(size_t i = 0; auto& j : buffers)
        {
            vertex_buffers[i++] = j.get_vk_buffer();
        }
        if(!offset)
        {
            VkDeviceSize offsets[]{};
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
                                                            *primary_counter:
                                                            secondary_counter->first]()
    {

        vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::set_scissors(VkRect2D scissors)
{
    delegate.push_back([this, scissors, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]()
    {

        vkCmdSetScissor(command_buffer, 0, 1, &scissors);
    });
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::reset_current_com_buffer()
{
    auto command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first;
    vkResetCommandBuffer(command_buffer, 0);
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::reset_pool()
{
    thread_resource->com_pool->reset_pool();
    return this;
}

void harpy::nest::command_commander::submit(VkPipelineStageFlags wait_dst_mask, threading::fence* wait_fence, threading::semaphore* render_finish_semaphore, threading::semaphore* wait_semaphore, bool do_wait)
{
    auto& submit_info = thread_resource->submit_infos;
    if(wait_dst_mask == VK_PIPELINE_STAGE_HOST_BIT)
        throw utilities::harpy_little_error("Oh, you tried to use abnormal wait destination mask"
            " for submit action. Check VUID-VkSubmitInfo-pWaitDstStageMask-00078");

    submit_info.waitSemaphoreCount = wait_semaphore ? 1 : 0;
    submit_info.pWaitSemaphores = wait_semaphore ? &wait_semaphore->get_vk_semaphore() : nullptr;
    submit_info.pWaitDstStageMask = wait_semaphore ? &wait_dst_mask : nullptr;

    submit_info.signalSemaphoreCount = render_finish_semaphore ? 1 : 0;
    submit_info.pSignalSemaphores = render_finish_semaphore ? &render_finish_semaphore->get_vk_semaphore() : nullptr;


    HARPY_VK_CHECK(vkQueueSubmit(
        thread_resource->queue,
        1,
        &submit_info,
        wait_fence ? wait_fence->get_vk_fence() : nullptr
        ));
    
    if(do_wait)
        HARPY_VK_CHECK(vkQueueWaitIdle(thread_resource->queue));
}

harpy::nest::command_commander* harpy::nest::command_commander::end_recording_secondary(bool do_write_to_primary)
{
    delegate.push_back(vkEndCommandBuffer, secondary_counter->first);
    is_writing_to_primary = do_write_to_primary;
    return this;
}

harpy::utilities::delegate harpy::nest::command_commander::end_recording()
{
    delegate.push_back(vkEndCommandBuffer, *primary_counter);
        thread_resource->submit_infos.pCommandBuffers = used_buffers.data();
        thread_resource->submit_infos.commandBufferCount = used_buffers.size();
    return delegate;
}

void harpy::nest::command_commander::fast_load_mesh(D3::mesh &mesh, std::vector<wrappers::vertex> &vertices,
    std::vector<uint32_t> &indices) {
    auto& mesh_vertices = mesh.get_vertices();
    auto& mesh_indices = mesh.get_indices();
    if(mesh_vertices.get_size() == 0)
        mesh_vertices.init(vertices.size());
    if(mesh_indices.get_size() == 0)
        mesh_indices.init(indices.size());

    reset_pool()
    ->start_recording()
    ->load_into_buffer(mesh_vertices, vertices)
    ->load_into_buffer(mesh_indices, indices)
    ->end_recording()();
    submit();
}

void harpy::nest::command_commander::fast_load_texture(texturing::texture &texture, utilities::image &image) {
    reset_pool()
    ->start_recording()
    ->load_into_texture(texture, image)
    ->end_recording()();
    submit();
}


harpy::nest::command_commander *harpy::nest::command_commander::trim_pool() {
    thread_resource->com_pool->trim_pool();
    return this;
}

harpy::nest::command_commander *harpy::nest::command_commander::bind_descriptor_sets(VkDescriptorSet set, pipeline::graphics_pipeline& pipeline) {

    delegate.push_back([this, set, pipeline = &pipeline, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]()
                       {
                           vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get_vk_layout(), 0, 1, &set, 0, nullptr);
                       });

    return this;
}

harpy::nest::command_commander *harpy::nest::command_commander::bind_descriptor_sets(std::vector<VkDescriptorSet>& sets,
                                                                                     harpy::nest::pipeline::graphics_pipeline &pipeline) {
    delegate.push_back([this, sets = &sets, pipeline = &pipeline, command_buffer = is_writing_to_primary ?
                                                            *primary_counter:
                                                            secondary_counter->first]()
                       {
                           vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->get_vk_layout(), 0, sets->size(), sets->data(), 0, nullptr);
                       });
    return this;
}

//TODO: unhardcode
harpy::nest::command_commander * harpy::nest::command_commander::bind_push_constants(pipeline::graphics_pipeline& pipe, glm::mat4 matrix) {
    delegate.push_back([this, matrix, &pipe, command_buffer = is_writing_to_primary ?
                                                                *primary_counter:
                                                                secondary_counter->first]() mutable
                           {
                               vkCmdPushConstants(command_buffer, pipe.get_vk_layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &matrix);
                           });
    return this;
}


