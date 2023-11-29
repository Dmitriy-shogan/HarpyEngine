#pragma once
#ifndef HARPY_NEST_SHADER_MODULE
#define HARPY_NEST_SHADER_MODULE
#include <any>
#include <string>
#include <volk/volk.h>
#include <nest/initializations/inititalizations.h>

namespace harpy::nest::shaders
{
    enum class shader_types : uint8_t
    {
        he_first = 1,

        vertex = 1,
        fragment,
        compute,
        geometry,
        tesc,
        tese,

        he_bm = 0,
        he_count = 6,
        
    };
    class shader_module
    {
        VkShaderModule shader{};
        shader_types type{};
    public:
        shader_module(shader_types);

        void init(const spirv_compilation_result& data);
        void init(const std::string& shader_assembly);
        
        
        VkShaderModule& get_shader();
        shader_types get_type();
        operator VkShaderModule&();

        ~shader_module();
    };

    struct standard_shader_set
    {
        shader_module vertex{shader_types::vertex};
        shader_module fragment{shader_types::fragment};
        shader_module tess_control{shader_types::tesc};
        shader_module tess_eval{shader_types::tese};
        shader_module compute{shader_types::compute};
        shader_module geometry{shader_types::geometry};
        std::any extra_info{};
    };
}

#endif//HARPY_NEST_SHADER_MODULE