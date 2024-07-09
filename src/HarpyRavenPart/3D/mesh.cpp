#include <3D/mesh.h>

#include <threading/thread_pool.h>

const harpy::nest::wrappers::data_buffer& harpy::nest::D3::mesh::get_vertices() const
{
    return vertices;
}

const harpy::nest::wrappers::data_buffer& harpy::nest::D3::mesh::get_indices() const
{
    return indices;
}

void harpy::nest::D3::mesh::load(
    command_commander& commander,
    std::vector<wrappers::vertex>& vertices,
    const std::vector<uint32_t>& indices,
    resources::vulkan_synchronisation_resource& resource)
{
    this->vertices = wrappers::data_buffer(wrappers::buffer_type::vertex, vertices.size());
    this->indices = wrappers::data_buffer(wrappers::buffer_type::indice, indices.size());

    auto delegate = commander.reset_pool()
    ->start_recording()
    ->load_vertex_index_buffers(
        vertices.data(),
        vertices.size() * sizeof(wrappers::vertex),
        indices,
        this->vertices.get_vk_buffer(),
        this->indices.get_vk_buffer()
    )
    ->end_recording();
    
    auto is_ready = utilities::threading::thread_pool<>::get_singleton().enqueue(delegate);

    while(!is_ready.get())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    commander.submit_one(std::move(resource), {});
}

bool harpy::nest::D3::mesh::is_loaded()
{
    return vertices.get_vk_buffer();
}
