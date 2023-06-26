#pragma once
#ifndef HARPY_CAMERA_LAYOUT
#define HARPY_CAMERA_LAYOUT

#include <utilities/harpy_little_error.h>

#include "interfaces/interfaces.h"

namespace harpy::nest::camera 
{
    class camera_layout : public interfaces::IRotatable, public interfaces::IMoveable
    {
        glm::vec3 coordinates;
        glm::mat4 view;
        
    public:
        camera_layout() : coordinates(glm::vec3(2.0f, 2.0f, 2.0f)), view(glm::lookAt(coordinates, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))) {}

        void move(glm::vec3 vec) override
        {
            coordinates += vec;
            view = glm::lookAt(coordinates, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        void move(float x, float y, float z) override
        {
            coordinates += glm::vec3{x, y, z};
            view = glm::lookAt(coordinates, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }

        void rotate(float angle, glm::vec3 vec) override
        {
            view = glm::rotate(view, glm::radians(angle), vec);
        }
        void rotate(float angle, float x, float y, float z) override
        {
            view = glm::rotate(view, glm::radians(angle), glm::vec3{x,y,z});
        }

        glm::mat4 get_view(){ return view; }

        

        ~camera_layout() override = default;
    };
}

#endif //HARPY_CAMERA_LAYOUT