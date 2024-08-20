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
        std::u8string id{u8"Model id #smth"};
        nest::texturing::texture texture{};

    public:

        model() = default;

        void bind_mesh(mesh const& msh);

        void set_id(std::u8string id);
        std::u8string get_id();

        void set_texture(std::u8string path);

        mesh& get_mesh();



    };

} // D3

#endif //HARPY_3D_MODEL
