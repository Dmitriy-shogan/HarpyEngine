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

        mesh msh{};
        glm::mat4 model_matrix{1.0f};
        nest::texturing::texture texture{};

        sz::string id{"Model id #smth"};

        sz::string pipeline_id{"standard"};

        sz::string mesh_loading_path{};
        sz::string texture_loading_path{};



    public:

        model() = default;

        void set_id(sz::string_view id);
        sz::string get_id();

        void set_texture(sz::string_view path);

        mesh& get_mesh();

        model* rotate(float angle, bool x, bool y, bool z);
        model* move(float x, float y, float z);
        model* scale(float x, float y, float z);

        void set_pipeline_id(sz::string_view pipe_id);
        sz::string_view get_pipeline_id();


    };

} // D3

#endif //HARPY_3D_MODEL
