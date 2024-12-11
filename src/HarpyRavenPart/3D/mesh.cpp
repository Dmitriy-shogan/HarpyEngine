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
    return vertices.get_size() == 0 && indices.get_size() == 0;
}

