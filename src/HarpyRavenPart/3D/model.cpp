//
// Created by Иван on 10.08.2024.
//

#include "3D/model.h"

#include "glm/gtx/transform.hpp"

namespace harpy::D3 {
    void model::set_id(sz::string_view id) {
        this->id = id;
    }

    sz::string model::get_id() {
        return id;
    }

    void model::set_texture(sz::string_view path) {
        texture_loading_path = path;
    }

    mesh& model::get_mesh() {
        return msh;
    }

    model* model::rotate(float angle, bool x, bool y, bool z) {
        model_matrix = glm::rotate(model_matrix, glm::radians(angle), {x, y, z});
    }

    model* model::move(float x, float y, float z) {
        model_matrix = glm::translate(model_matrix, {x, y, z});
    }

    model* model::scale(float x, float y, float z) {
        model_matrix = glm::scale(model_matrix, {x, y, z});
    }

    void model::set_pipeline_id(sz::string_view pipe_id) {
        pipeline_id = pipe_id;
    }

    sz::string_view model::get_pipeline_id() {
        return pipeline_id;
    }
} // harpy