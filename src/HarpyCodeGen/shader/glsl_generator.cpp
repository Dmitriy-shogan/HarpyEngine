//
// Created by Dima on 02.08.2024.
//

#include <shader/glsl_generator.hpp>

#include <map>
#include <version>
#include <logger/harpy_little_error.h>

using namespace harpy::codegen;

bool harpy::codegen::operator==(const glsl_variable &lhs, const glsl_variable &rhs) {
    return lhs.name == rhs.name &&
           lhs.type == rhs.type &&
           lhs.value == rhs.value &&
           lhs.is_local == rhs.is_local &&
           lhs.in == rhs.in &&
           lhs.is_const == rhs.is_const &&
           lhs.is_uniform == rhs.is_uniform &&
           lhs.array_size == rhs.array_size &&
           lhs.location == rhs.location &&
           lhs.binding == rhs.binding;
}

harpy::codegen::glsl_generator* glsl_generator::start_generate() {
    result.clear();
    variables_data.clear();
    functions.clear();
    structs_data.clear();
    return this;
}

ashvardanian::stringzilla::string harpy::codegen::glsl_generator::stop_generate() {
    result.append(version);
    for(auto& i : structs_data) {
        if(i.is_uniform) {
            if(i.binding < 0)
                throw utilities::harpy_little_error("Tried to create a uniform buffer object without binding");
            result.append("layout(binding = ");
            result.append(std::to_string(i.binding));
            result.append(") uniform Uniform");
            result.append(i.name);
        } else {
            result.append("struct ");
            result.append(i.name);
        }
        result.append("\n{\n");
        for(auto& f :i.vars) {
            result.append(f.type);
            result.append(" ");
            result.append(f.name);
            result.append(";\n");
        }
        if(i.is_uniform) {
            result.append("} ");
            result.append(i.uniform_name);
            result.append(";\n");
        } else {
            result.append("};\n");
        }
    }

    int location_counter{0};
    for(auto& i : variables_data) {
        if(i.is_local) {
            result.append(i.type);
            result.append(" ");
            result.append(i.name);
            result.append(";\n");
            continue;
        }
        result.append("layout(location = ");
        if(i.location >= 0) {
            result.append(std::to_string(i.location));
        } else {
            result.append(std::to_string(location_counter));
            if(i.type.contains('d') && i.type != "double") {
                /*
                 * This is just a some kine of bullshit
                 * First, most of the dedicated videocards REALLY don't like doubles -- up to x32 times SLOWER with them, then with float
                 * Second, non dedicated videocards are do not support doubles at all
                 * Third, all of the functions of glsl do math in floats
                 */
                throw utilities::harpy_little_error("Sorry, but doubles, except \"double\", in shaders do not work very well in most devices. Please do not use them");
            }
            if(i.type.front() != 'm') {
                location_counter += 1;
            } else {
                location_counter += 2;
            }
        }
        result.append(") ");
        if(!i.is_local) {
            if(i.in)
                result.append("in ");
            else
                result.append("out ");
        }
        result.append(i.type);
        result.append(" ");
        result.append(i.name);
        result.append(";\n");
    }

    functions.push_back(main_function);
    for(auto& i : functions) {
        result.append(i.return_value);
        result.append(" ");
        result.append(i.name);
        result.append("(");
        for(auto& j : i.parameters) {

            if(j.in && j.is_local)
                result.append("inout");
            else if(j.is_local)
                result.append("out");

            result.append(j.type);
            result.append(" ");
            result.append(j.name);
            if(j != i.parameters.back()) {
                result.append(", ");
            }
        }
        result.append(")\n{\n");
        result.append(i.body);
        result.append("\n}");
    }
    functions.pop_back();
    return result;
}
        glsl_generator* glsl_generator::set_prefix(sz::string_view prefix) {
    this->prefix = prefix;
    return this;
}
        glsl_generator* glsl_generator::set_type(sz::string_view type) {
    shader_type = type;
    return this;
}
        glsl_generator* glsl_generator::set_name(sz::string_view name) {
    shader_name = name;
    return this;
}

        glsl_generator* glsl_generator::set_main_function_name(sz::string_view main){ main_function.name = main; return this; }

        glsl_generator* glsl_generator::layout(){variables_data.back().is_local == false; return this; }

        glsl_generator* glsl_generator::new_struct(sz::string name){structs_data.push_back(glsl_struct{.name = name}); return this; }

glsl_generator * glsl_generator::struct_add_variable(glsl_variable variable) {
    structs_data.back().vars.push_back(variable);
    return this;
}

glsl_generator * glsl_generator::struct_uniform_name(sz::string_view uniform_name) {
    structs_data.back().uniform_name = uniform_name;
    return this;
}

glsl_generator * glsl_generator::struct_binding(int binding) {
    structs_data.back().binding = binding;
    return this;
}

glsl_generator * glsl_generator::struct_is_uniform(bool is_uniform) {
    structs_data.back().is_uniform = is_uniform;
    return this;
}

glsl_generator* glsl_generator::new_variable(sz::string name){variables_data.push_back(glsl_variable{.name = name}); return this; }
        glsl_generator* glsl_generator::variable_name(sz::string_view name){variables_data.back().name = name; return this; }
        glsl_generator* glsl_generator::variable_type(sz::string_view type){variables_data.back().type = type; return this; }
        glsl_generator* glsl_generator::variable_value(sz::string_view value){variables_data.back().value = value; return this; }
        glsl_generator* glsl_generator::variable_is_local(bool is_local){variables_data.back().is_local = is_local; return this; }
        glsl_generator* glsl_generator::variable_is_in(bool is_in){variables_data.back().in = is_in; variables_data.back().is_local = false; return this; }
        glsl_generator* glsl_generator::variable_is_out(bool is_out){variables_data.back().in != is_out; variables_data.back().is_local = false; return this; }
        glsl_generator* glsl_generator::variable_is_const(bool is_const){variables_data.back().is_const = is_const; return this; }
        glsl_generator* glsl_generator::variable_is_uniform(bool is_uniform){variables_data.back().is_uniform = is_uniform; return this; }
        glsl_generator* glsl_generator::variable_array(int array_size){variables_data.back().array_size = array_size; return this; }
        glsl_generator* glsl_generator::variable_location(int location){variables_data.back().location = location; return this; }
        glsl_generator* glsl_generator::variable_binding(int binding){variables_data.back().binding = binding; return this; }

        glsl_generator* glsl_generator::start_function(sz::string_view name){functions.push_back(function_description{.name = name, .body = {"{\n"}});
    return this; }

glsl_generator * glsl_generator::start_write_main() {
    functions.push_back(main_function);
    return this;
}

glsl_generator * glsl_generator::end_write_main() {
    main_function = std::move(functions.back());
    functions.pop_back();
    return this;
}

glsl_generator* glsl_generator::function_add_parameter(sz::string_view type, sz::string_view name, bool is_in, bool is_out) {
    functions.back().parameters.push_back(glsl_variable{.name = name, .type = type, .is_local = is_out, .in = is_in});
    return this;
}
        glsl_generator* glsl_generator::function_write(glsl_variable variable){ functions.back().body.append(variable.name); return this; }
        glsl_generator* glsl_generator::function_write(glsl_struct structure){ functions.back().body.append(structure.name);return this; }
        glsl_generator* glsl_generator::function_write(int value){ functions.back().body.append(std::to_string(value)); return this; }
        glsl_generator* glsl_generator::function_write(float value){ functions.back().body.append(std::to_string(value)); return this; }
        glsl_generator* glsl_generator::function_write(double value){ functions.back().body.append(std::to_string(value)); return this; }
        glsl_generator* glsl_generator::function_write_raw(sz::string_view raw){ functions.back().body.append(raw); return this; }
        glsl_generator* glsl_generator::function_write_raw_line(sz::string_view raw){ functions.back().body.append(raw).append(";\n"); return this; }

        glsl_generator* glsl_generator::function_assignment(){functions.back().body.append(" = "); return this; }
        glsl_generator* glsl_generator::function_equals(){functions.back().body.append(" == "); return this; }
        glsl_generator* glsl_generator::function_plus(){functions.back().body.append(" + "); return this; }
        glsl_generator* glsl_generator::function_minus(){functions.back().body.append(" - "); return this; }
        glsl_generator* glsl_generator::function_multiply(){functions.back().body.append(" * "); return this; }
        glsl_generator* glsl_generator::function_texture(sz::string_view raw){ functions.back().body.append(" texture(");
    functions.back().body.append(raw);
    functions.back().body.append(")");return this; }

        glsl_generator* glsl_generator::function_vec(int size, sz::string_view raw){functions.back().body.append("vec" + std::to_string(size) + "(" + raw.data() + ")"); return this; }

        glsl_generator* glsl_generator::function_cos(float value){functions.back().body.append("cos(" + std::to_string(value) + ")"); return this; }
        glsl_generator* glsl_generator::function_sin(float value){functions.back().body.append("sin(" + std::to_string(value) + ")"); return this; }
        glsl_generator* glsl_generator::function_tan(float value){functions.back().body.append("tan(" + std::to_string(value) + ")"); return this; }
        glsl_generator* glsl_generator::function_abs(float value){functions.back().body.append("abs(" + std::to_string(value) + ")"); return this; }
        glsl_generator* glsl_generator::function_radians(float value){functions.back().body.append("radians(" + std::to_string(value) + ")"); return this; }
        glsl_generator* glsl_generator::function_degrees(float value){functions.back().body.append("degrees(" + std::to_string(value) + ")"); return this; }

glsl_generator* glsl_generator::function_cos(sz::string_view raw){functions.back().body.append("cos(").append(raw).append(")"); return this; }
glsl_generator* glsl_generator::function_sin(sz::string_view raw){functions.back().body.append("sin(").append(raw).append(")"); return this; }
glsl_generator* glsl_generator::function_tan(sz::string_view raw){functions.back().body.append("tan(").append(raw).append(")"); return this; }
glsl_generator* glsl_generator::function_abs(sz::string_view raw){functions.back().body.append("abs(").append(raw).append(")"); return this; }
glsl_generator* glsl_generator::function_radians(sz::string_view raw){functions.back().body.append("radians(").append(raw).append(")"); return this; }
glsl_generator* glsl_generator::function_degrees(sz::string_view raw){functions.back().body.append("degrees(").append(raw).append(")"); return this; }

        glsl_generator* glsl_generator::function_vec_cross(sz::string_view raw){ functions.back().body.append("cross(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_distance(sz::string_view raw){ functions.back().body.append("distance(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_dot(sz::string_view raw){ functions.back().body.append("dot(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_equal(sz::string_view raw){ functions.back().body.append("equal(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_length(sz::string_view raw){ functions.back().body.append("length(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_normalize(sz::string_view raw){ functions.back().body.append("normalize(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_reflect(sz::string_view raw){ functions.back().body.append("reflect(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_vec_refract(sz::string_view raw){ functions.back().body.append("refract(").append(raw).append(")");return this; }

        glsl_generator* glsl_generator::function_mat_deter(sz::string_view raw){ functions.back().body.append("determinant(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_mat_inverse(sz::string_view raw){ functions.back().body.append("inverse(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_mat_transpose(sz::string_view raw){ functions.back().body.append("transpose(").append(raw).append(")");return this; }
        glsl_generator* glsl_generator::function_mat_outer_prod(sz::string_view raw){ functions.back().body.append("outerProduct(").append(raw).append(")");return this; }

        glsl_generator* glsl_generator::function_gl_position(){functions.back().body.append("gl_Position"); return this; }
        glsl_generator* glsl_generator::function_gl_clip_distance(){functions.back().body.append("gl_ClipDistance"); return this; }
        glsl_generator* glsl_generator::function_gl_cull_distance(){functions.back().body.append("gl_CullDistance"); return this; }
        glsl_generator* glsl_generator::function_gl_frag_coord(){functions.back().body.append("gl_FragCoord"); return this; }
        glsl_generator* glsl_generator::function_gl_frag_depth(){functions.back().body.append("gl_FragDepth"); return this; }
        glsl_generator* glsl_generator::function_gl_instance_id(){functions.back().body.append("gl_InstanceID"); return this; }
        glsl_generator* glsl_generator::function_gl_sample_id(){functions.back().body.append("gl_SampleID"); return this; }


        glsl_generator* glsl_generator::function_line_end(){functions.back().body.append("\n"); return this; }
        glsl_generator* glsl_generator::function_semicolumn(){ functions.back().body.append(";");return this; }

        glsl_generator* glsl_generator::end_function_body(){functions.back().body.append("}\n");
 return this; }

std::vector<glsl_variable> & glsl_generator::get_variables_data() {
    return variables_data;
}

std::vector<glsl_struct> & glsl_generator::get_structs_data() {
    return structs_data;
}
