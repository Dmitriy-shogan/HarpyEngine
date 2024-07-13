//
// Created by Иван on 11.07.2024.
//

#ifndef HARPY_3D_UNIFORM_BUFFER_OBJECTS
#define HARPY_3D_UNIFORM_BUFFER_OBJECTS
#include <glm/glm.hpp>

namespace harpy::D3::uniform_buffers{
    struct sight_ub{
        glm::mat4 model{}, view{}, proj{};
    };
}











#endif //HARPY_3D_UNIFORM_BUFFER_OBJECTS
