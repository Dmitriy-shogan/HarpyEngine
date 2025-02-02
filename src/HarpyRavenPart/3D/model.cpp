//
// Created by Xyeveloper on 10.08.2024.
//

#include "3D/model.h"

#include "glm/gtx/transform.hpp"

namespace harpy::D3 {
    model::model(sz::string_view id, sz::string_view texture_id, sz::string_view pipeline_id)
        : id(id), material_id(pipeline_id) {
        all_models[id] = this;
    }

    void model::set_id(sz::string_view id) {
        all_models.erase(this->id);
        this->id = id;
        all_models[id] = this;
    }

    sz::string model::get_id() {
        return id;
    }

    void model::set_texture_file(sz::string_view path) {
        texture_loading_path = path;
    }

    void model::set_main_texture_id(sz::string_view id) {
        textures.front().set_id(id);
    }

    sz::string model::get_main_texture_id() {
        return textures.front().get_id();
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
        material_id = pipe_id;
    }

    sz::string model::get_pipeline_id() {
        return material_id;
    }

    model::~model() {
        if(id != "default")
            all_models.erase(id);
    }

    void model::set_mesh_file(sz::string_view path) {
        mesh_loading_path = path;
    }
} // harpy