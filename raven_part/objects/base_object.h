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
    //TESTING BLOCK
    nest::vulkan_spinal_cord& vulkan_backend;
    nest::pools::command_pool& pool;
    
    hbuffers::vertex_buffer vertices{pool, vulkan_backend};
    hbuffers::index_buffer indices{pool, vulkan_backend};
    std::vector<hbuffers::uniform_buffer> uniforms;
    
    
    
    
protected:
    glm::mat4 model{1.0f};
    
public:
    base_object(nest::vulkan_spinal_cord& cord, nest::pools::command_pool& com_pool) : vulkan_backend(cord), pool(com_pool)
    {
        uniforms.resize(MAX_FRAMES_IN_FLIGHT, {pool, vulkan_backend});
    }

    void init()
    {
        vertices.init();
        indices.init();
        for(auto& i : uniforms)
        {
            i.init();
        }
    }

    hbuffers::vertex_buffer& get_vertex_buffer(){return vertices;}
    hbuffers::index_buffer& get_index_buffer(){return indices;}
    std::vector<hbuffers::uniform_buffer>& get_uniform_buffers(){return uniforms;}
    glm::mat4 get_model(){return model;}

    void rotate(float angle, int x, int y, int z) 
    {
        model = glm::rotate(model, glm::radians(angle), glm::vec3(x, y, z));
    }
    void rotate(float angle, glm::vec3 vec) 
    {
        model = glm::rotate(model, glm::radians(angle),vec);
    };
    void move(int x, int y, int z) 
    {
        model = glm::translate(model,  glm::vec3(x, y, z));
    };
    void move(glm::vec3 vec) 
    {
        model = glm::translate(model, vec);
    };
    void scale(int x, int y, int z) 
    {
        model = glm::scale(model, glm::vec3(x, y, z));
    };
    void scale(glm::vec3 vec) 
    {
        model = glm::scale(model, vec);
    };
};

}

#endif