#pragma once
#ifndef HARPY_BASE_OBJECT
#define HARPY_BASE_OBJECT

#include <nest/buffers/vertex_buffer.h>
#include <nest/buffers/index_buffer.h>

namespace harpy::objects{
   namespace hbuffers = harpy::nest::buffers;
class base_object
{
    glm::mat4 model{1.0f};
    hbuffers::vertex_buffer vertices;
    hbuffers::index_buffer indices;

    
public:
    
};

}

#endif