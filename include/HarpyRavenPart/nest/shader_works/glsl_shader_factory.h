#pragma once
#ifndef HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_GLSL
#define HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_GLSL

#include <future>
#include <map>
#include <nest/shader_works/shader_module.h>

#define SHADERC_SHAREDLIB
#include <shaderc/shaderc.hpp>
#include <nest/inititalizations.h>

namespace harpy::nest::shaders
{
    //TODO: create tool for saving shaderc compiler options
    struct glsl_shader_factory_options
    {
        enum glsl_shader_bools : uint16_t
        {
            he_first = 0,
            
            shaderc_optimization_level_size = 1 << 0,
            shaderc_optimization_level_performance = 1 << 1,
            suppress_warnings = 1 << 2,
            warnings_as_errors = 1 << 3,
            autobind_uniforms = 1 << 4,
            autosample_textures = 1 << 5,
            //preserve_bindings = 1 << 6, //broken bit for now
            automap_locations = 1 << 7,
            invert_y = 1 << 8,
            nan_clamp = 1 << 9,
            generate_debug = 1 << 10,
            force_version = 1 << 11, // NOT IMPLEMENTED
            
            he_is_bm = 1,
            he_count = 12
        };
        std::map<sz::string, sz::string> definitions{};
        int forced_version{}; // NOT IMPLEMENTED
        uint16_t flags{};
        //So, HSO file.
        //It contains
        //flags in 2 bytes
        //Forced version if contains so
        //Definitions in a way like MACROS::DEFINITION
        //SEPARATOR_SEPARATOR_SEPARATOR
        //Repeat till you save all options
    };
    

    
    class glsl_shader_factory
    {
        //TODO: make something about singletone-ism
        HARPY_MAKE_SINGLETON(glsl_shader_factory)
        
        shaderc::Compiler compiler{};
        size_t current_set{0};
        std::vector<glsl_shader_factory_options> sets{};
        std::mutex mutex{};

        shaderc::CompileOptions process_options();

        static shader_types process_extension(sz::string_view filename);
        shaderc_shader_kind shader_types_to_shaderc_shader_kind(shader_types type);
    public:

        sz::string preprocess( sz::string_view shader, shader_types type, bool do_save = false);
        sz::string preprocess_from_file( sz::string_view filename, bool do_save = false);
        
        spirv_compilation_result compile(sz::string_view shader, shader_types type, bool do_save = false);
        spirv_compilation_result full_compilation(sz::string_view shader, shader_types type, bool do_save = false);
        spirv_compilation_result compile_from_file(sz::string_view filename, bool do_save = false);
        spirv_compilation_result full_compilation_from_file(sz::string_view filename, bool do_save = false);

        spirv_compilation_result build_assembly( sz::string_view assembled_string);
        sz::string compile_binary( sz::string_view shader, shader_types type, bool do_save = false);
        sz::string full_compilation_binary(sz::string_view shader, shader_types type, bool do_save = false);
        sz::string compile_binary_from_file( sz::string_view filename, bool do_save = false);
        sz::string full_compilation_binary_from_file( sz::string_view filename, bool do_save = false);
        
        //MUST BE VALID HSO FILE
        void load_sets( sz::string_view filename = "options.hso");
        
        void save_sets( sz::string_view filename = "options.hso");

        glsl_shader_factory_options& get_set(int index);
        
        void set_set(const glsl_shader_factory_options& set);
        void set_set(size_t index);
        
        shader_module create_shader_module( sz::string_view filename);
        shader_module create_shader_module(spirv_compilation_result shader, shader_types type);
        
        static std::future<shader_module> create_shader_module_async(sz::string_view filename);
        
        //FILE MUST BE VALID HGSP FILE WITH OPTIONS BLOCK
        static std::future<shader_set> create_shader_set_async(sz::string_view filename);
        
        shader_set create_shader_set(sz::string_view way_to_file); //HarpyGlslShaderPackage
    };

    
}

#endif//HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_GLSL