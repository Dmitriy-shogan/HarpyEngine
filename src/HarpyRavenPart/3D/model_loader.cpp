//
// Created by Xyeveloper on 10.08.2024.
//

#include "3D/model_loader.h"
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::unique_ptr<harpy::D3::model> harpy::D3::model_loader::load_model(sz::string_view path, sz::string_view id,  sz::string_view pipeline_id) {
    std::unique_ptr<model> result{new model()};
    result->set_id(id);
    result->set_pipeline_id(pipeline_id);

    uint32_t import_flags = 0;

    import_flags |= aiProcess_ValidateDataStructure; // validates the imported scene data structure.
    import_flags |= aiProcess_Triangulate;           // triangulates all faces of all meshes.
    import_flags |= aiProcess_SortByPType;           // splits meshes with more than one primitive type in homogeneous sub-meshes
    import_flags |= aiProcess_FlipUVs;

    // generate missing normals or UVs
    import_flags |= aiProcess_CalcTangentSpace; // calculates  tangents and bitangents
    import_flags |= aiProcess_GenSmoothNormals; // ignored if the mesh already has normals
    import_flags |= aiProcess_GenUVCoords;      // converts non-UV mappings (such as spherical or cylindrical mapping) to proper texture coordinate channels

    import_flags |= aiProcess_RemoveRedundantMaterials;
    import_flags |= aiProcess_JoinIdenticalVertices;
    import_flags |= aiProcess_FindInstances;



    auto scene = importer.ReadFile(path.cbegin(), import_flags);


    if (scene->HasMeshes()) {
        auto *local_mesh = scene->mMeshes[0];
        mesh actual_mesh{};

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
        commander.fast_load_mesh(actual_mesh, vertices, indices);
        result->msh = std::move(actual_mesh);
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

