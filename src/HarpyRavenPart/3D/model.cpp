//
// Created by Иван on 10.08.2024.
//

#include "3D/model.h"

#include "glm/gtx/transform.hpp"

namespace harpy::D3 {
    model::model(sz::string_view id, sz::string_view mesh_id, sz::string_view texture_id, sz::string_view pipeline_id)
        : id(id), mesh_id(mesh_id), texture_id(texture_id), pipeline_id(pipeline_id) {

    }

    void model::set_id(sz::string_view id) {
        this->id = id;
    }

    sz::string model::get_id() {
        return id;
    }

    void model::set_texture(sz::string_view path) {
        texture_loading_path = path;
    }

    void model::set_texture_id(sz::string_view id) {
        texture_id = id;
    }

    sz::string model::get_texture_id() {
        return texture_id;
    }

    void model::set_mesh_id(sz::string_view id) {
        mesh_id = id;
    }

    sz::string model::get_mesh_id() {
        return mesh_id;
    }

    model* model::rotate(float angle, bool x, bool y, bool z) {
        model_matrix = glm::rotate(model_matrix, glm::radians(angle), {x, y, z});
        return this;
    }

    model* model::move(float x, float y, float z) {
        model_matrix = glm::translate(model_matrix, {x, y, z});
        return this;
    }

    model* model::scale(float x, float y, float z) {
        model_matrix = glm::scale(model_matrix, {x, y, z});
        return this;
    }

    void model::set_pipeline_id(sz::string_view pipe_id) {
        pipeline_id = pipe_id;
    }

    sz::string model::get_pipeline_id() {
        return pipeline_id;
    }
} // harpy