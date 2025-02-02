//
// Created by Xyeveloper on 10.08.2024.
//

#ifndef HARPY_3D_MODEL
#define HARPY_3D_MODEL
#include <3D/mesh.h>
#include <nest/texturing/texture.h>


namespace harpy::D3 {

    class model {
        friend class model_loader;
        friend class renderer;

        glm::mat4 model_matrix{1.0f};
        std::vector<mesh> meshes{};
        std::vector<nest::texturing::texture> textures{};

        sz::string id{"Model id #smth"};
        sz::string material_id{"default"};

        sz::string mesh_loading_path{};
        sz::string texture_loading_path{};
    public:
        static inline std::unordered_map<sz::string, model*> all_models{};

        model(sz::string_view id, sz::string_view texture_id = "default", sz::string_view pipeline_id = "default");

        void set_id(sz::string_view id);
        sz::string get_id();

        void set_pipeline_id(sz::string_view pipe_id);
        sz::string get_pipeline_id();

        void set_texture_file(sz::string_view path);
        void set_mesh_file(sz::string_view path);

        void set_main_texture_id(sz::string_view id);
        sz::string get_main_texture_id();

        model* rotate(float angle, bool x, bool y, bool z);
        model* move(float x, float y, float z);
        model* scale(float x, float y, float z);

        ~model();
    };

} // D3

#endif //HARPY_3D_MODEL
