#pragma once
#ifndef HARPY_BASE_OBJECT
#define HARPY_BASE_OBJECT

#include <nest/buffers/vertex_buffer.h>
#include <nest/buffers/index_buffer.h>
#include <nest/buffers/uniform_buffer.h>

namespace harpy::objects{
   namespace hbuffers = nest::buffers;
class base_object
{
    hbuffers::vertex_buffer vertices{pool, vulkan_backend};
    hbuffers::index_buffer indices{pool, vulkan_backend};
    std::vector<hbuffers::uniform_buffer> uniforms;
    
    //TESTING BLOCK
    nest::vulkan_spinal_cord& vulkan_backend;
    nest::pools::command_pool& pool;
    
    
protected:
    glm::mat4 model{1.0f};
    

    
public:
    base_object(nest::vulkan_spinal_cord& cord, nest::pools::command_pool& com_pool) : vulkan_backend(cord), pool(com_pool)
    {
        uniforms.resize(MAX_FRAMES_IN_FLIGHT, {pool, vulkan_backend});
    }
    
};

}

#endif