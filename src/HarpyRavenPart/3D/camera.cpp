//
// Created by Xyeveloper on 14.08.2024.
//
#define GLM_ENABLE_EXPERIMENTAL
#include "3D/camera.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <nest/windowing/input_controller.h>

namespace harpy::D3{
    camera::camera(glm::vec3 position) : position(position){

    }

    camera::camera(float x, float y, float z) : position({x, y, z}){

    }

    void camera::move(glm::vec3 position){
        this->position += position;
    }
    void camera::move(float x, float y, float z){
        position += glm::vec3{x, y, z};
    }
    void camera::set_position(glm::vec3 position){
        this->position  = position;
    }
    void camera::set_position(float x, float y, float z){
        position = {x, y, z};
    }

    void camera::rotate_horizontally(float degree){
        horizontal_rotation += degree;
    }
    void camera::rotate_vertically(float degree){
        vertical_rotation += degree;
    }
    void camera::set_hor_rotation(float degree){
        horizontal_rotation = degree;
    }
    void camera::set_ver_rotation(float degree){
        vertical_rotation = degree;
    }

    void camera::bind_update_action(utilities::delegate delegate){
        update_action = std::move(delegate);
    }
    utilities::delegate camera::unbind_update_action(){
        auto deleg = std::move(update_action);
        update_action = utilities::delegate{};
        return deleg;
    }
    bool camera::update(){
        return update_action.invoke();
    }

    glm::mat4 camera::construct_view_matrix(){
        return glm::inverse(glm::translate(glm::mat4{1.0f}, position) * construct_rotation_matrix());
    }
    glm::mat4 camera::construct_rotation_matrix(){
        glm::quat ver_rot = glm::angleAxis(vertical_rotation, glm::vec3 { 1.f, 0.f, 0.f });
        glm::quat hor_rot = glm::angleAxis(horizontal_rotation, glm::vec3 { 0.f, -1.f, 0.f });

        return glm::toMat4(hor_rot) * glm::toMat4(ver_rot);
    }
    glm::mat4 camera::construct_perspective_matrix(float aspect_ratio){
        auto perspective_matrix = glm::perspective(glm::radians(fov_degrees), aspect_ratio, 0.01f, 10000.f);
        perspective_matrix[1][1] *= -1;
        return perspective_matrix;
    }

    glm::vec3& camera::get_position(){
        return position;
    }
    glm::vec3& camera::get_center(){
        return center;
    }
    glm::vec3& camera::get_velocity(){
        return velocity;
    }

    void camera::set_fov(float degrees){
        fov_degrees = degrees;
    }
    float camera::get_fov(){
        return fov_degrees;
    }

    void camera::set_vertical_restrictions(float low_y, float high_y){

    }
    void camera::set_horizontal_restrictions(float low_z, float high_z){

    }

    std::pair<float, float> camera::get_vertical_restrictions(){
        return {0,0};
    }
    std::pair<float, float> camera::get_horizontal_restrictions(){
        return {0,0};
    }

    void camera::set_standard_movement(camera &cam, float speed, nest::windowing::input_controller &controller) {
        static utilities::delegate move_delegate{};
        if(move_delegate.size() == 0)
            move_delegate.push_back([&cam, speed](){
            cam.get_position() += glm::vec3(cam.construct_rotation_matrix() * glm::vec4(cam.get_velocity() * speed, 0.f));
        });

        cam.bind_update_action(move_delegate);

        static utilities::delegate forwards{}, forwards_release{};
        static utilities::delegate backwards{}, backwards_release{};
        static utilities::delegate left{}, right{};
        static utilities::delegate left_relese{}, right_relese{};
        if(forwards.size() == 0) {
            forwards.push_back([&cam](){
                cam.get_velocity().z = -1;
            });
            forwards_release.push_back([&cam](){
                cam.get_velocity().z = 0;
            });
            backwards.push_back([&cam](){
                cam.get_velocity().z = 1;
            });
            backwards_release.push_back([&cam](){
                cam.get_velocity().z = 0;
            });
            left.push_back([&cam](){
                cam.get_velocity().x = -1;
            });
            left_relese.push_back([&cam](){
                cam.get_velocity().x = 0;
            });
            right.push_back([&cam](){
                cam.get_velocity().x = 1;
            });
            right_relese.push_back([&cam](){
                cam.get_velocity().x = 0;
            });
        }

        controller.map_key(GLFW_KEY_W, nest::windowing::input_action::press, forwards);
        controller.map_key(GLFW_KEY_S, nest::windowing::input_action::press, backwards);
        controller.map_key(GLFW_KEY_W, nest::windowing::input_action::release, forwards_release);
        controller.map_key(GLFW_KEY_S, nest::windowing::input_action::release, backwards_release);
        controller.map_key(GLFW_KEY_A, nest::windowing::input_action::press, left);
        controller.map_key(GLFW_KEY_D, nest::windowing::input_action::press, right);
        controller.map_key(GLFW_KEY_A, nest::windowing::input_action::release, left_relese);
        controller.map_key(GLFW_KEY_D, nest::windowing::input_action::release, right_relese);
    }

    void camera::set_standard_mouse_movement(camera&  cam, float sensitivity, nest::windowing::input_controller& input) {
        static utilities::parameters_delegate<double*, double*> mouse_movement{};
        auto center = input.get_window()->get_screen_center();

        mouse_movement.push_back([&cam, &last_x = center.first, &last_y = center.second](double* x, double* y) mutable {
            float xoffset = *x - last_x;
            float yoffset = *y - last_y;
            last_x = *x;
            last_y = *y;

            const float sensitivity = 0.0001f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            cam.get_vertical_rotation() -= yoffset;
            cam.get_hor_rotation() += xoffset;
        });
        input.map_mouse_movement(mouse_movement);
        input.disable_cursor();
        input.turn_raw_mouse_motion(true);
    }

    float &camera::get_hor_rotation() {
        return horizontal_rotation;
    }

    float &camera::get_vertical_rotation() {
        return vertical_rotation;
    }

    float &camera::get_roll_rotation() {
        return roll_rotation;
    }

    float &camera::get_yaw() {
        return horizontal_rotation;
    }

    float &camera::get_pitch() {
        return  vertical_rotation;
    }

    float &camera::get_roll() {
        return  roll_rotation;
    }
}
