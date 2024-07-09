#pragma once
#ifndef HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_HLSL
#define HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_HLSL
#include <future>
#include <nest/shader_works/shader_module.h>
#include <nest/inititalizations.h>

namespace harpy::nest::shaders
{
    //TODO: create tool for saving shaderc compiler options
    struct hlsl_shader_factory_options
    {
        std::vector<std::string> definitions{};
        
    };
    
    class hlsl_shader_factory
    {
        //TODO: make something about singletone-ism
        HARPY_MAKE_SINGLETON(hlsl_shader_factory)

        int id{0};
        size_t current_set{0};
        std::vector<hlsl_shader_factory_options> sets{};
        std::mutex mutex{};
    
    public:
        
        std::vector<uint32_t> compile(std::string shader, bool do_save = false);
        std::vector<uint32_t> compile_from_file(std::string filename, bool do_save = false);
        
        void load_set(std::string filename, bool set_current = true);
        void load_sets(std::string directory);

        void save_set(std::string filename);
        void save_sets(std::string directory);

        hlsl_shader_factory_options& get_set(int index);
        
        void set_set(hlsl_shader_factory_options set);
        void set_set(size_t index);
        
        shader_module create_shader_module(std::string filename);
        shader_module create_shader_module(std::vector<uint32_t> shader);
        
        static std::future<std::vector<shader_module>> create_shader_modules_async(std::string filename);
        
        //FILE MUST BE VALID HSP FILE
        static std::future<std::vector<shader_set>> create_shader_sets_async(std::string filename);
        
        //FILE MUST BE VALID HSP FILE
        shader_set create_shader_set_from_hsp(std::string filename); //HarpyShaderPackage
        //FILE MUST BE VALID HSP FILE
        shader_set create_shader_set(std::string directory);
    };
}

#endif//HARPY_NEST_SHADER_TOOLS_SHADER_FACTORY_HLSL

