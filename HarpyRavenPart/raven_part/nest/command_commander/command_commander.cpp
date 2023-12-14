#include "command_commander.h"
#include "nest/buffers/data_buffer.h"
#include <string>

#include "nest/buffers/vertex_buffer.h"
#include "nest/vulkan_threading/primitives/fence.h"

harpy::nest::command_commander::command_commander(VkDevice* device,
                                                  VmaAllocator* allocator) : device(device), allocator(allocator)
{
}

void harpy::nest::command_commander::bind_thread_res(std::shared_ptr<resources::command_thread_resource> res)
{
    thread_resource = res;
}

void harpy::nest::command_commander::bind_records_cis(resources::records_cis cis)
{
    records_resource = cis;
}

void harpy::nest::command_commander::unbind_thread_res()
{
    thread_resource.reset();
}

void harpy::nest::command_commander::unbind_records_cis()
{
    records_resource.reset();
}

harpy::nest::command_commander* harpy::nest::command_commander::start_recording(long starting_buffer)
{
    if(!records_resource || !thread_resource)
        throw utilities::error_handling::harpy_little_error("Please first bind both thread and record create infos resources before starting recording");
    
    delegate.clear();
    if(starting_buffer >= 0)
        primary_counter = starting_buffer;
    else
        primary_counter = thread_resource->com_pool->get_primary_buffers().size() - 1;
    
    delegate.push_back(
            vkBeginCommandBuffer,
            thread_resource->com_pool->get_primary_buffers()[primary_counter],
            &records_resource->primary_begin_ci
            );
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::record_to_secondary(long starting_buffer, long primary_buffer_index)
{
    if (starting_buffer >= 0)
        primary_counter = starting_buffer;
    else
        primary_counter = thread_resource->com_pool->get_secondary_buffers().size() - 1;
   
    thread_resource->com_pool->get_secondary_buffers().back().parent_primary_buffer = primary_buffer_index;
    delegate.push_back(
            vkBeginCommandBuffer,
            thread_resource->com_pool->get_secondary_buffers()[primary_counter].buffer,
            &records_resource->secondary_begin_ci);

    is_writing_to_primary = false;
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

harpy::nest::command_commander* harpy::nest::command_commander::load_into_buffer(VkBuffer dst, void* data, std::size_t size)
{
    if(thread_resource->queue_type == wrappers::graphics || thread_resource->queue_type == wrappers::compute)
    {
        utilities::error_handling::logger::get_logger().log(utilities::error_handling::error_severity::warning, "WARNING!, you're trying to copy buffer in a graphics or compute queue. Please, use transfer only, main or universal");
        utilities::error_handling::logger::get_logger().show_last_log();
    }
    buffers::data_buffer src{};
    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    ci.size = size;
    ci.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    //Just for now, needs research
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.pNext = nullptr;

    VmaAllocationCreateInfo alloc_ci{};
    alloc_ci.usage = VMA_MEMORY_USAGE_AUTO;
    alloc_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    HARPY_VK_CHECK(vmaCreateBuffer(*allocator, &ci, &alloc_ci, &src.get_vk_buffer(), &src.get_vma_allocation(), nullptr));

    void* temp_data{nullptr};
    vmaMapMemory(*allocator, src.get_vma_allocation(), &temp_data);
    std::memcpy(temp_data, data, size);
    vmaUnmapMemory(*allocator, src.get_vma_allocation());
    
    records_resource->copy_cis.emplace_back(0, 0, size);
    
    delegate.push_back(vkCmdCopyBuffer,
        thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer,
        src.get_vk_buffer(),
        dst,
        1,
        &records_resource->copy_cis.back());
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::draw_indexed(
    uint32_t indexex_to_draw,
    uint32_t instances_to_draw,
    uint32_t starting_index,
    uint32_t starting_instance,
    uint32_t offset
    )
{
    if(is_writing_to_primary)
    {
        delegate.push_back(
        vkCmdDrawIndexed,
            thread_resource->com_pool->get_primary_buffers()[primary_counter],
            indexex_to_draw,
            instances_to_draw,
            starting_index,
            offset,
            starting_instance
            );
    } else
    {
        delegate.push_back(
        vkCmdDrawIndexed,
            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer,
            indexex_to_draw,
            instances_to_draw,
            starting_index,
            offset,
            starting_instance
            );
    }
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_index_buffer(VkBuffer buffer, uint32_t offset)
{
    if(is_writing_to_primary)
        delegate.push_back(
            vkCmdBindIndexBuffer,
            thread_resource->com_pool->get_primary_buffers()[primary_counter],
            buffer,
            offset,
            VK_INDEX_TYPE_UINT32);
    else
        delegate.push_back(
            vkCmdBindIndexBuffer,
            thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer,
            buffer,
            offset,
            VK_INDEX_TYPE_UINT32);
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::bind_vertex_buffers(std::vector<VkBuffer>& buffers, uint32_t thirst_binding, VkDeviceSize* offset)
{
    if(is_writing_to_primary)
        delegate.push_back(
            vkCmdBindVertexBuffers,
            thread_resource->com_pool->get_primary_buffers()[primary_counter],
            thirst_binding,
            buffers.size(),
            buffers.data(),
            offset);
    else
        delegate.push_back(
                    vkCmdBindVertexBuffers,
                    thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer,
                    thirst_binding,
                    buffers.size(),
                    buffers.data(),
                    offset);
    return this;
}

harpy::nest::command_commander* harpy::nest::command_commander::set_viewport()
{
    return this;
}

void harpy::nest::command_commander::submit(std::future<bool>& is_completed, threading::primitives::fence fence)
{
    is_completed.wait();
    HARPY_VK_CHECK(vkQueueSubmit(
        *thread_resource->queue.get(),
        records_resource->submit_cis.size(),
        records_resource->submit_cis.data(),
        fence
        ));
}

harpy::nest::command_commander* harpy::nest::command_commander::end_recording_secondary()
{
    delegate.push_back(vkEndCommandBuffer, thread_resource->com_pool->get_secondary_buffers()[secondary_counter].buffer);
    return this;
}

harpy::utilities::delegate harpy::nest::command_commander::end_recording(bool do_clean_resources)
{
    //REFACTOR THIS SHIT
    delegate.push_back(vkEndCommandBuffer, thread_resource->com_pool->get_primary_buffers()[primary_counter]);
    records_resource->submit_cis.front().pCommandBuffers = &thread_resource->com_pool->get_primary_buffers()[primary_counter];
    if(do_clean_resources)
    {
        thread_resource.reset();
    }
    return delegate;
}
