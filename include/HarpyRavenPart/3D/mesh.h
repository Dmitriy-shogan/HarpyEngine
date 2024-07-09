#pragma once
#ifndef HARPY_NEST_3D_MESH
#define HARPY_NEST_3D_MESH
#include <nest/wrappers/buffers/data_buffer.h>

#include <nest/command_commander/command_commander.h>

namespace harpy::nest::D3
{
    class mesh
    {
        wrappers::data_buffer vertices{wrappers::buffer_type::vertex};
        wrappers::data_buffer indices{wrappers::buffer_type::indice};
    
    public:

        mesh() = default;

        void load(
            command_commander& commander,
            std::vector<wrappers::vertex>& vertices,
            const std::vector<uint32_t>& indices,
            resources::vulkan_synchronisation_resource& resource);

        bool is_loaded();

        const wrappers::data_buffer& get_vertices() const;
        const wrappers::data_buffer& get_indices() const;
    };
}


#endif //HARPY_NEST_3D_MESH