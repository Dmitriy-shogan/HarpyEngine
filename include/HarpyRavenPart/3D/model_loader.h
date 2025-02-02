//
// Created by Xyeveloper on 10.08.2024.
//

#ifndef HARPY_D3_MODEL_LOADER
#define HARPY_D3_MODEL_LOADER
#include <assimp/Importer.hpp>
#include <3D/model.h>
#include <nest/command_commander/command_commander.h>
#include "assimp/postprocess.h"

namespace harpy::D3 {

    class model_loader {
        Assimp::Importer importer{};
        nest::command_commander commander{};
        inline static constinit uint32_t import_flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace |
                aiProcess_ValidateDataStructure | aiProcess_SortByPType | aiProcess_GenSmoothNormals |
                aiProcess_GenUVCoords | aiProcess_JoinIdenticalVertices | aiProcess_FindInstances;

    public:
        model_loader(std::unique_ptr<nest::resources::command_thread_resource> thread_res);

        [[nodiscard]] mesh load_mesh(sz::string_view path, sz::string_view id, aiScene* valid_scene = nullptr);
        [[nodiscard]] model load_model(sz::string_view path, sz::string_view id, aiScene* valid_scene = nullptr);
        void update_model(model &model);

    };
}


#endif //HARPY_D3_MODEL_LOADER
