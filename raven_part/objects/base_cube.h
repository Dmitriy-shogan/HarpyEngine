#pragma once
#ifndef HARPY_OBJECTS_TEST_CUBE
#define HARPY_OBJECTS_TEST_CUBE
#include <objects/base_object.h>

namespace harpy::objects
{
    class base_cube : public interfaces::ITransformable, public base_object
    {
        
    public:
        
        void rotate(float angle, int x, int y, int z) override
        {
            model = glm::rotate(model, glm::radians(angle), glm::vec3(x, y, z));
        }
        void rotate(float angle, glm::vec3 vec) override
        {
            model = glm::rotate(model, glm::radians(angle),vec);
        };
        void move(float angle, int x, int y, int z) override
        {
            model = glm::translate(model,  glm::vec3(x, y, z));
        };
        void move(float angle, glm::vec3 vec) override
        {
            model = glm::translate(model, vec);
        };
        void scale(float angle, int x, int y, int z) override
        {
            model = glm::scale(model, glm::vec3(x, y, z));
        };
        void scale(float angle, glm::vec3 vec) override
        {
            model = glm::scale(model, vec);
        };
    };
}

#endif //HARPY_OBJECTS_TEST_CUBE