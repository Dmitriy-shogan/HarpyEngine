//
// Created by Xyeveloper on 14.08.2024.
//
#define GLM_ENABLE_EXPERIMENTAL
#include "3D/camera.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
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
        auto perspective_matrix = glm::perspective(glm::radians(fov_degrees), aspect_ratio, 10000.f, 0.01f);
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
