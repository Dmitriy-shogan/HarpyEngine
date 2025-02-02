//
// Created by Xyeveloper on 10.08.2024.
//

#include "3D/model_loader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

harpy::D3::mesh harpy::D3::model_loader::load_mesh(sz::string_view path, sz::string_view id, aiScene* valid_scene) {
    mesh result{};

    auto scene = valid_scene ? valid_scene : importer.ReadFile(path.cbegin(), import_flags);
    if (scene->HasMeshes()) {
        auto *local_mesh = scene->mMeshes[0];

        std::vector<nest::wrappers::vertex> vertices{local_mesh->mNumVertices};
        std::vector<uint32_t> indices{};

        indices.resize(local_mesh->mNumFaces * 3);
        for (int i = 0; i < local_mesh->mNumFaces; i++) {

            auto indice_index = i*3;
            indices[indice_index] = local_mesh->mFaces[i].mIndices[0];
            indices[indice_index + 1] = local_mesh->mFaces[i].mIndices[1];
            indices[indice_index + 2] = local_mesh->mFaces[i].mIndices[2];
        }

        for (int i = 0; i < local_mesh->mNumVertices; ++i) {
            vertices[i].coords = {local_mesh->mVertices[i].x, local_mesh->mVertices[i].y, local_mesh->mVertices[i].z};
            vertices[i].texture_coords = {local_mesh->mTextureCoords[0][i].x, local_mesh->mTextureCoords[0][i].y};
            vertices[i].normals = {local_mesh->mNormals[i].x, local_mesh->mNormals[i].y, local_mesh->mNormals[i].z};
        }
        commander.fast_load_mesh(result, vertices, indices);
        return result;
    }

    throw utilities::harpy_little_error("So, your model was a fucking crap without meshes");
}

harpy::D3::model_loader::model_loader(std::unique_ptr<nest::resources::command_thread_resource> thread_res) {
    // remove points and lines
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE | aiPrimitiveType_POINT);

    // remove cameras
    importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_CAMERAS);

    commander.bind_thread_res(std::move(thread_res));
}

void harpy::D3::model_loader::update_model(harpy::D3::model &model) {


}

harpy::D3::model harpy::D3::model_loader::load_model(sz::string_view path, sz::string_view id, aiScene* valid_scene) {
    model md{id};
    auto scene = valid_scene ? valid_scene : importer.ReadFile(path.cbegin(), import_flags);
    if(scene->HasMeshes()){
        for (int i = 0; i < scene->mNumMeshes; i++) {
            md.meshes.emplace_back(load_mesh(path, id, valid_scene));
        }
    }
    if(scene->HasTextures()) {
        for (int i = 0; i < scene->mNumTextures; i++) {
            auto *texture = scene->mTextures[i];
            texture->pcData;
        }
    }

    return model{id};
}