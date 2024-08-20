//
// Created by Xyeveloper on 14.08.2024.
//

#ifndef HARPY_3D_CAMERA
#define HARPY_3D_CAMERA
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <util/delegate.h>

namespace harpy::D3 {
    class camera {
        glm::vec3 position{};
        glm::vec3 center{};
        glm::vec3 velocity{};

        float fov_degrees{90};

        //Yaw
        float horizontal_rotation{-90};
        //Pitch
        float vertical_rotation{};
        //Roll
        float roll_rotation{};

        std::pair<float, float> vertical_restrictions{0, 0};
        std::pair<float, float> horizontal_restrictions{0, 0};

        utilities::delegate update_action{};


    public:

        camera(glm::vec3 position);
        camera(float x, float y, float z);

        void move(glm::vec3 position);
        void move(float x, float y, float z);
        void set_position(glm::vec3 position);
        void set_position(float x, float y = 0, float z = 0);

        void rotate_horizontally(float degree);
        void rotate_vertically(float degree);
        void set_hor_rotation(float degree);
        void set_ver_rotation(float degree);

        void bind_update_action(utilities::delegate delegate);
        utilities::delegate unbind_update_action();
        bool update();

        glm::mat4 construct_view_matrix();
        glm::mat4 construct_rotation_matrix();
        glm::mat4 construct_perspective_matrix(float aspect_ratio);

        glm::vec3& get_position();
        glm::vec3& get_center();
        glm::vec3& get_velocity();

        float& get_hor_rotation();
        float& get_vertical_rotation();
        float& get_roll_rotation();

        float& get_yaw();
        float& get_pitch();
        float& get_roll();


        void set_fov(float degree);
        float get_fov();

        void set_vertical_restrictions(float low_y, float high_y);
        void set_horizontal_restrictions(float low_z, float high_z);

        std::pair<float, float> get_vertical_restrictions();
        std::pair<float, float> get_horizontal_restrictions();


    };

}
#endif //HARPY_3D_CAMERA
