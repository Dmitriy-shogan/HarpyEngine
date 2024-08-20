//
// Created by Xyeveloper on 10.08.2024.
//

#ifndef HARPY_D3_MODEL_LOADER
#define HARPY_D3_MODEL_LOADER
#include <assimp/Importer.hpp>
#include <3D/model.h>

namespace harpy::D3 {

    class model_loader {
        Assimp::Importer importer{};
        nest::command_commander commander{};

        void load_mesh(model& model, aiScene& scene);

    public:
        model_loader(std::unique_ptr<nest::resources::command_thread_resource> thread_res);

        std::unique_ptr<model> load_model(std::string_view path);

    };
}


#endif //HARPY_D3_MODEL_LOADER
