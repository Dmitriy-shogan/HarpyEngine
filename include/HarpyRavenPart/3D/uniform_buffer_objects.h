//
// Created by Xyeveloper on 11.07.2024.
//

#ifndef HARPY_3D_UNIFORM_BUFFER_OBJECTS
#define HARPY_3D_UNIFORM_BUFFER_OBJECTS
#include <glm/glm.hpp>

//TODO: better and more uniform templates
namespace harpy::D3::uniform_buffers {
    struct sight_ub{
        glm::mat4 view{1.0f}, proj{1.0f}, view_proj{1.0f};
    };
    struct light_data_ub{
        float specular_strength{0.0f};
        glm::vec3 light_pos{0.0f}, camera_pos{0.0f};
        glm::vec4 light_color{0.0f};
    };
}











#endif //HARPY_3D_UNIFORM_BUFFER_OBJECTS
