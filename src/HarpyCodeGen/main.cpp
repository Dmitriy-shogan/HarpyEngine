//
// Created by Xyeveloper on 01.08.2024.
//
#include <shader/glsl_generator.hpp>
#include <logger/logger.h>


using namespace harpy;
using namespace codegen;

int main() {
    glsl_generator generator{};
    auto& logger {harpy::utilities::logger::get_logger()};
    logger << generator.start_generate()
    ->new_struct("UnifromBufferObject")->struct_binding(0)->struct_is_uniform(true)->struct_uniform_name("ubo")
    ->struct_add_variable({"model", "mat4"})->struct_add_variable({"view", "mat4"})->struct_add_variable({"proj", "mat4"})
    ->new_variable("inPosition")->variable_location(0)->variable_type("vec3")->variable_is_in(true)
    ->new_variable("inNormals")->variable_location(1)->variable_type("vec3")->variable_is_in(true)
    ->new_variable("inTexCoord")->variable_location(2)->variable_type("vec2")->variable_is_in(true)
    ->new_variable("inColor")    ->variable_location(3)->variable_type("vec3")->variable_is_in(true)
    ->new_variable("fragColor")->variable_location(0)->variable_type("vec3")->variable_is_out(true)
    ->new_variable("fragTexCoord")->variable_location(1)->variable_type("vec2")->variable_is_out(true)
    ->start_write_main()
    ->function_gl_position()->function_assignment()->function_write_raw("ubo.proj * ubo.view * ubo.model")
    ->function_multiply()->function_vec(4, "inPosition, 1.0")->function_semicolumn()->function_line_end()
    ->function_write_raw_line("fragColor = inColor;")
    ->function_write_raw_line("fragTexCoord = inTexCoord;")
    ->end_write_main()
    ->stop_generate();
    return 0;
}
