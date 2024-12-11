#pragma once
#ifndef HARPY_3D_MESH
#define HARPY_3D_MESH
#include "nest/wrappers/data_buffer.h"

namespace harpy::D3
{
    class mesh
    {
        nest::wrappers::data_buffer vertices{nest::wrappers::buffer_type::vertex};
        nest::wrappers::data_buffer indices{nest::wrappers::buffer_type::indice};

    
    public:

        mesh() = default;

        bool is_loaded();

        nest::wrappers::data_buffer & get_vertices();
        nest::wrappers::data_buffer & get_indices();
    };
}


#endif //HARPY_3D_MESH