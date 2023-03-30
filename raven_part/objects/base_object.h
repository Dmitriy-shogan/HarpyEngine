#pragma once
#ifndef HARPY_BASE_OBJECT
#define HARPY_BASE_OBJECT

#include <nest/primitive_data_types/buffers/vertex_buffer.h>
#include <nest/primitive_data_types/buffers/index_buffer.h>

namespace harpy_object{
    using namespace harpy_nest;
class base_object
{
    static glm::mat4 view;
    glm::mat4 model{1.0f};
    vertex_buffer vertices;
    index_buffer indices;

    
public:
    
};

}

#endif