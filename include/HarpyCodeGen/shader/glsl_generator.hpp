//
// Created by Dima on 02.08.2024.
//

#ifndef HARPY_CODEGEN_GLSL_GENERATOR
#define HARPY_CODEGEN_GLSL_GENERATOR
#include <fstream>
#include <variant>
#include <stringzilla/stringzilla.hpp>

#include "codegen_dll_macro.h"

namespace harpy::codegen {
    namespace sz = ashvardanian::stringzilla;

    //TODO: components
    struct glsl_variable {
        sz::string name{"yoy"};
        sz::string type{"float"};
        sz::string value{"0.0f"};
        bool is_local{true};
        bool in{false};
        bool is_const{false};
        bool is_uniform{false};
        int array_size{1};
        int location{-1}; //-1 means autolocate
        int binding{0};
    };

    HARPY_CODEGEN_API bool operator==(const glsl_variable& lhs, const glsl_variable& rhs);

    struct glsl_struct {
        sz::string name{"base_struct"};
        std::vector<glsl_variable> vars{};
        bool is_uniform{false};
        int binding{-1};
        sz::string uniform_name{"ubo"};
    };

    struct function_description {
        sz::string name{""};
        sz::string return_value{"void"};
        std::vector<glsl_variable> parameters{};
        sz::string body{};
    };

    class HARPY_CODEGEN_API glsl_generator {
        static inline const sz::string version{"#version 450\n"};
        sz::string prefix{};
        sz::string shader_type{".vert"};
        sz::string shader_name{"standard"};
        function_description main_function{"main", "void"};

        sz::string result{};

        std::vector<glsl_variable> variables_data{};
        std::vector<glsl_struct> structs_data{};
        std::vector<function_description> functions{};
    public:

        glsl_generator* start_generate();
        sz::string stop_generate();

        glsl_generator* set_prefix(sz::string_view prefix = "");
        glsl_generator* set_type(sz::string_view type = "vert");
        glsl_generator* set_name(sz::string_view name = "standard");
        glsl_generator* set_main_function_name(sz::string_view main = "main");

        glsl_generator* layout();

        sz::string name{"base_struct"};
        std::vector<glsl_variable> vars{};
        bool is_uniform{false};
        int binding{-1};
        sz::string uniform_name{"ubo"};

        glsl_generator* new_struct(sz::string name);
        glsl_generator* struct_add_variable(glsl_variable variable);
        glsl_generator* struct_uniform_name(sz::string_view uniform_name);
        glsl_generator* struct_binding(int binding);
        glsl_generator* struct_is_uniform(bool is_uniform);

        glsl_generator* new_variable(sz::string name);
        glsl_generator* variable_name(sz::string_view name);
        glsl_generator* variable_type(sz::string_view type);
        glsl_generator* variable_value(sz::string_view value);
        glsl_generator* variable_is_local(bool is_local = false);
        glsl_generator* variable_is_in(bool is_in = false);
        glsl_generator* variable_is_out(bool is_out = false);
        glsl_generator* variable_is_const(bool is_const);
        glsl_generator* variable_is_uniform(bool is_uniform);
        glsl_generator* variable_array(int array_size);
        glsl_generator* variable_location(int location);
        glsl_generator* variable_binding(int binding);

        glsl_generator* start_function(sz::string_view name);

        glsl_generator* start_write_main();
        glsl_generator* end_write_main();

        glsl_generator* function_add_parameter(sz::string_view type, sz::string_view name, bool is_in = false, bool is_out = false);
        glsl_generator* function_write(glsl_variable variable);
        glsl_generator* function_write(glsl_struct structure);
        glsl_generator* function_write(int value);
        glsl_generator* function_write(float value);
        glsl_generator* function_write(double value);
        glsl_generator* function_write_raw(sz::string_view raw);
        glsl_generator* function_write_raw_line(sz::string_view raw);

        glsl_generator* function_assignment();
        glsl_generator* function_equals();
        glsl_generator* function_plus();
        glsl_generator* function_minus();
        glsl_generator* function_multiply();
        glsl_generator* function_texture(sz::string_view raw);
        glsl_generator* function_vec(int size, sz::string_view raw);

        glsl_generator* function_cos(float value);
        glsl_generator* function_sin(float value);
        glsl_generator* function_tan(float value);
        glsl_generator* function_abs(float value);
        glsl_generator* function_radians(float value);
        glsl_generator* function_degrees(float value);

        glsl_generator* function_cos(sz::string_view raw);
        glsl_generator* function_sin(sz::string_view raw);
        glsl_generator* function_tan(sz::string_view raw);
        glsl_generator* function_abs(sz::string_view raw);
        glsl_generator* function_radians(sz::string_view raw);
        glsl_generator* function_degrees(sz::string_view raw);

        glsl_generator* function_vec_cross(sz::string_view raw);
        glsl_generator* function_vec_distance(sz::string_view raw);
        glsl_generator* function_vec_dot(sz::string_view raw);
        glsl_generator* function_vec_equal(sz::string_view raw);
        glsl_generator* function_vec_length(sz::string_view raw);
        glsl_generator* function_vec_normalize(sz::string_view raw);
        glsl_generator* function_vec_reflect(sz::string_view raw);
        glsl_generator* function_vec_refract(sz::string_view raw);

        glsl_generator* function_mat_deter(sz::string_view raw);
        glsl_generator* function_mat_inverse(sz::string_view raw);
        glsl_generator* function_mat_transpose(sz::string_view raw);
        glsl_generator* function_mat_outer_prod(sz::string_view raw);

        glsl_generator* function_gl_position();
        glsl_generator* function_gl_clip_distance();
        glsl_generator* function_gl_cull_distance();
        glsl_generator* function_gl_frag_coord();
        glsl_generator* function_gl_frag_depth();
        glsl_generator* function_gl_instance_id();
        glsl_generator* function_gl_sample_id();

        glsl_generator* function_swizzling(sz::string_view raw);

        glsl_generator* function_line_end();
        glsl_generator* function_semicolumn();

        glsl_generator* end_function_body();

        std::vector<glsl_variable>& get_variables_data();
        std::vector<glsl_struct>& get_structs_data();



    };
}


#endif //HARPY_CODEGEN_GLSL_GENERATOR
