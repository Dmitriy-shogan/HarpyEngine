//
// Created by Xyeveloper on 10.08.2024.
//

#ifndef HARPY_3D_MODEL
#define HARPY_3D_MODEL
#include <3D/mesh.h>


namespace harpy::D3 {

    class model {
        friend class model_loader;

        mesh msh{};

        nest::texturing::texture texture{};
        std::u8string id{u8"Model id #smth"};

        std::u8string mesh_loading_path{};
        std::u8string texture_loading_path{};

    public:

        model() = default;

        void set_id(std::u8string id);
        std::u8string get_id();

        void set_texture(std::u8string path);

        mesh& get_mesh();



    };

} // D3

#endif //HARPY_3D_MODEL
