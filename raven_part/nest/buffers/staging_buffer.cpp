#include "staging_buffer.h"
#include <buffers/vertex_buffer.h>
#include <buffers/index_buffer.h>
void harpy::nest::buffers::staging_buffer::copy_into_buffer(vertex_buffer& buff)
{
    void* data;
    vkMapMemory(pool.get_vk_device(), buffer_mem, 0, buffer_size, 0, &data);
    memcpy(data, buff.get_vertices().data(), (size_t) buffer_size);
    vkUnmapMemory(pool.get_vk_device(), buffer_mem);
    copy_buffer(buff, pool);
    
}
void harpy::nest::buffers::staging_buffer::copy_into_buffer(index_buffer& buff)
{
    void* data;
    vkMapMemory(pool.get_vk_device(), buffer_mem, 0, buffer_size, 0, &data);
    memcpy(data, buff.get_indices().data(), (size_t) buffer_size);
    vkUnmapMemory(pool.get_vk_device(), buffer_mem);
    copy_buffer(buff, pool);
}

void harpy::nest::buffers::staging_buffer::copy_info_any(void* ptr)
{
    void* data;
    vkMapMemory(pool.get_vk_device(), buffer_mem, 0, buffer_size, 0, &data);
    memcpy(data, ptr, (size_t) buffer_size);
    vkUnmapMemory(pool.get_vk_device(), buffer_mem);
}
