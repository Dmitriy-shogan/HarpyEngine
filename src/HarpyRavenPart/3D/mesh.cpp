#include <3D/mesh.h>

#include <threading/thread_pool.h>

 harpy::nest::wrappers::data_buffer&  harpy::D3::mesh::get_vertices()
{
    return vertices;
}

harpy::nest::wrappers::data_buffer& harpy::D3::mesh::get_indices()
{
    return indices;
}


bool harpy::D3::mesh::is_loaded()
{
    return vertices.get_vk_buffer();
}

void harpy::D3::mesh::load(harpy::nest::command_commander &commander, std::vector<nest::wrappers::vertex> &vertices,
                                 std::vector<uint32_t> &indices) {
    if(!this->vertices.get_vk_buffer())
        this->vertices.init(vertices.size());
    if(!this->indices.get_vk_buffer())
        this->indices.init(indices.size());

    commander.reset_pool()
    ->start_recording()
    ->load_into_buffer(this->vertices, vertices)
    ->load_into_buffer(this->indices, indices)
    ->end_recording()();
    commander.submit();


}
