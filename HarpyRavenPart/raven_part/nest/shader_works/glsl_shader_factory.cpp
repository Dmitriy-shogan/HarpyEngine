﻿#include "glsl_shader_factory.h"
#include <util/util.h>
#include <threading/thread_pool.h>

#include <utility>

#include "nest/managers/swapchain_manager.h"

using namespace harpy::nest::shaders;
const std::string separator {" SEPARATOR_SEPARATOR "};
const std::string shader_name {"shader"};

shaderc::CompileOptions glsl_shader_factory::process_options()
{
    shaderc::CompileOptions options;
    if(sets.empty())
    {
        sets.emplace_back();
        return options;
    }
    
    for(auto& i : sets.at(current_set).definitions)
    {
        if (i.second.empty())
            options.AddMacroDefinition(i.first);
        else
            options.AddMacroDefinition(i.first, i.second);
    }
    options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
     
    if(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::suppress_warnings)
        options.SetSuppressWarnings();
    
    if(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::warnings_as_errors)
        options.SetWarningsAsErrors();
    
    if(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::generate_debug)
        options.SetGenerateDebugInfo();

    options.SetInvertY(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::invert_y);
    options.SetAutoBindUniforms(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::autobind_uniforms);
    options.SetAutoSampledTextures(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::autosample_textures);
    options.SetAutoMapLocations(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::automap_locations);
    options.SetNanClamp(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::nan_clamp);

    if(sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::shaderc_optimization_level_performance)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }
    else if (sets[current_set].flags & glsl_shader_factory_options::glsl_shader_bools::shaderc_optimization_level_size)
    {
        options.SetOptimizationLevel(shaderc_optimization_level_size);
    }
    return options;
}

shader_types glsl_shader_factory::process_extension(std::string_view filename)
{
    //vert, frag, tesc, tese, geom, comp
    switch (filename.back())
    {
    case 't':
        return shader_types::vertex;
    case 'g':
        return shader_types::fragment;
    case 'c':
        return shader_types::tesc;
    case 'e':
        return shader_types::tese;
    case 'm':
        return shader_types::geometry;
    case 'p':
        return shader_types::compute;
        default:
            throw utilities::error_handling::harpy_little_error("Tossed not a shader or invalid shader extension");
    }
}

shaderc_shader_kind glsl_shader_factory::shader_types_to_shaderc_shader_kind(shader_types type)
{
    switch(type)
    {
    case shader_types::fragment:
        return shaderc_fragment_shader;
    case shader_types::compute:
        return shaderc_compute_shader;
    case shader_types::geometry:
        return shaderc_geometry_shader;
    case shader_types::tesc:
        return shaderc_tess_control_shader;
    case shader_types::tese:
        return shaderc_tess_evaluation_shader;
    case shader_types::vertex:
        return shaderc_vertex_shader;
        default:
            throw utilities::error_handling::harpy_little_error("Wrong shader type while converting tp shaderc types");
    }
}

void glsl_shader_factory::set_set(const glsl_shader_factory_options& set)
{
    for(int counter = 0; auto& i : sets)
    {
        if(set.definitions == i.definitions
            && set.flags == i.flags
            && set.forced_version == i.forced_version)
        {
            current_set = counter;
            return;
        }
        ++counter;
    }
    sets.push_back(set);
    current_set = sets.size() - 1;
}

void glsl_shader_factory::set_set(size_t index)
{
    if (index < 0 || index >= sets.size())
        throw utilities::error_handling::harpy_little_error("Wrong index while setting set for glsl shader factory");
    current_set = index;
}

std::string glsl_shader_factory::preprocess(const std::string& shader, shader_types type, bool do_save)
{
    auto result =
      compiler.PreprocessGlsl(shader, shader_types_to_shaderc_shader_kind(type),
          shader_name.c_str(), process_options());

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw utilities::error_handling::harpy_little_error(result.GetErrorMessage());
    }

    return {result.cbegin(), result.cend()};
}

harpy::spirv_compilation_result glsl_shader_factory::compile(const std::string& shader, shader_types type, bool do_save)
{
    
    shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(shader, shader_types_to_shaderc_shader_kind(type), "shader lul", process_options());

    if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw utilities::error_handling::harpy_little_error(module.GetErrorMessage());
    }

    return {module.cbegin(), module.cend()};
}

std::string glsl_shader_factory::compile_binary(const std::string& shader, shader_types type, bool do_save)
{
    auto result =
      compiler.CompileGlslToSpvAssembly(shader, shader_types_to_shaderc_shader_kind(type),
          shader.c_str(), process_options());

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw utilities::error_handling::harpy_little_error(result.GetErrorMessage());
    }
    return {result.cbegin(), result.cend()};
}

harpy::spirv_compilation_result glsl_shader_factory::full_compilation(std::string shader, shader_types type, bool do_save)
{
    auto processed = preprocess(std::move(shader), type);
    return compile(processed, type, do_save);
}

std::string glsl_shader_factory::full_compilation_binary(std::string shader, shader_types type, bool do_save)
{
    auto processed = preprocess(std::move(shader), type);
    return compile_binary(processed, type, do_save);
}

std::string glsl_shader_factory::preprocess_from_file(const std::string& filename, bool do_save)
{
    auto type = process_extension(filename);
    auto shader = utilities::read_file("shaders/glsl" + filename);
    return preprocess(std::move(shader), type, do_save);
}

harpy::spirv_compilation_result glsl_shader_factory::compile_from_file(const std::string& filename, bool do_save)
{
    auto type = process_extension(filename);
    auto shader = utilities::read_file("shaders/glsl" + filename);
    return compile(std::move(shader), type, do_save);
}

std::string glsl_shader_factory::compile_binary_from_file(const std::string& filename, bool do_save)
{
    auto type = process_extension(filename);
    auto shader = utilities::read_file(filename);
    return compile_binary(std::move(shader), type, do_save);
}

harpy::spirv_compilation_result glsl_shader_factory::full_compilation_from_file(const std::string& filename, bool do_save)
{
    auto type = process_extension(filename);
    auto shader = utilities::read_file(filename);
    return full_compilation(std::move(shader), type, do_save);
}

harpy::spirv_compilation_result glsl_shader_factory::build_assembly(const std::string& assembled_string)
{
    auto result = compiler.AssembleToSpv(assembled_string);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        throw utilities::error_handling::harpy_little_error(result.GetErrorMessage());
    }
    return {result.cbegin(), result.cend()};
}

std::string glsl_shader_factory::full_compilation_binary_from_file(const std::string& filename, bool do_save)
{
    auto type = process_extension(filename);
    auto shader = utilities::read_file(filename);
    return full_compilation_binary(std::move(shader), type, do_save);
}

void glsl_shader_factory::load_sets(const std::string& filename)
{
    auto data = utilities::read_file("shaders/glsl/" + filename);
    size_t iter = 0;
    size_t sep_iter{};
    while(iter < data.size() && iter != std::string::npos)
    {
        glsl_shader_factory_options options{};
        
        options.flags = static_cast<uint16_t>(std::stoul(data.substr(iter, data.find('\n', iter) - iter)));
        iter = data.find('\n', iter) + 1;
        
        options.forced_version =  static_cast<uint16_t>(std::stoul(data.substr(iter, data.find('\n', iter) - iter)));
        iter = data.find('\n', iter) + 1;

        sep_iter = data.find(separator, iter) - 2; //for some reason it don't like spaces, so we need to exclude them
        
        while(iter < sep_iter)
        {
            auto temp = data.substr(iter, data.find(':', iter) - iter);
            iter = data.find(':', iter) + 1;
            options.definitions[temp] = data.substr(iter, data.find('^', iter)  - iter);
            iter = data.find('^', iter) + 1;
        }
        
        sets.push_back(options);
        iter = data.find('\n', iter + separator.size()) + 1;
    }
    
}


void glsl_shader_factory::save_sets(const std::string& filename)
{
    std::string data{};
    for(auto& i : sets)
    {
        data.append(std::to_string(i.flags) + '\n');
        data.append(std::to_string(i.forced_version) + '\n');
        for(auto &f : i.definitions)
            data.append(f.first + ':' + f.second + '^');
        data.append('\n' + separator + '\n');
    }
    
    utilities::write_file("shaders/glsl/" + filename, data);
}

glsl_shader_factory_options& glsl_shader_factory::get_set(int index)
{return sets.at(index);}

shader_module glsl_shader_factory::create_shader_module(const std::string& filename)
{
    auto type = process_extension(filename);
    auto spv_shader = full_compilation_from_file(filename);

    shader_module module{type};
    module.init(spv_shader);
    return module;
}

shader_module glsl_shader_factory::create_shader_module(spirv_compilation_result shader, shader_types type)
{
    shader_module module{type};
    module.init(std::move(shader));
    return module;
}

std::future<shader_module> glsl_shader_factory::create_shader_module_async(std::string filename)
{
    return utilities::threading::std_thread_pool::get_singleton().enqueue
    ([file = std::move(filename)]() -> shader_module
    {
        return glsl_shader_factory::get_singleton().create_shader_module(file);
    });
}

std::future<standard_shader_set> glsl_shader_factory::create_shader_set_async(std::string filename)
{
    return utilities::threading::std_thread_pool::get_singleton().enqueue
    ([file = std::move(filename)]() -> standard_shader_set
    {
        return glsl_shader_factory::get_singleton().create_shader_set(file);
    });
}

standard_shader_set glsl_shader_factory::create_shader_set(std::string way_to_file)
{
    standard_shader_set main_set{};
    auto file = utilities::read_file(std::move(way_to_file));
    auto iter = file.find("###VERTEX###") + 12; //12 is the size of ###VERTEX###
    if(iter == std::string::npos)
        throw utilities::error_handling::harpy_little_error("Your HSO file don't contain vertex shader. That's shame");
    main_set.vertex.init(full_compilation(file.substr(iter, file.find("###", iter) - iter), shader_types::vertex));

    iter = file.find("###FRAGMENT###") + 13; //13 is the size of ###FRAGMENT###
    if(iter == std::string::npos)
        throw utilities::error_handling::harpy_little_error("Your HSO file don't contain fragment shader. That's shame");
    main_set.fragment.init(full_compilation(file.substr(iter, file.find("###", iter)  - iter), shader_types::fragment));
    
    if(file.find("###OPTIONS###"))
    {
        iter += 13; //13 is the size of ###OPTIONS###
        auto data = file.substr(iter, file.find("###", iter)  - iter);
        glsl_shader_factory_options options{};
        
        options.flags = static_cast<uint16_t>(std::stoul(data.substr(iter, data.find('\n', iter) - iter)));
        iter = data.find('\n', iter) + 1;
        
        options.forced_version =  static_cast<uint16_t>(std::stoul(data.substr(iter, data.find('\n', iter) - iter)));
        iter = data.find('\n', iter) + 1;

        auto sep_iter = data.find(separator, iter);
        
        while(iter < sep_iter)
        {
            auto temp = data.substr(iter, data.find(':', iter) - iter);
            iter = data.find(':', iter);
            options.definitions[temp] = data.substr(iter, data.find('^', iter)  - iter);
            iter = data.find('^', iter);
        }
        set_set(options);
    }
    if(file.find("###GEOMETRY###"))
    {
        iter += 14; //14 is size of ###GEOMETRY### 
        main_set.geometry.init(full_compilation(file.substr(iter, file.find("###", iter) - iter), shader_types::geometry));
    }
    if(file.find("###COMPUTE###"))
    {
        iter += 12; // 12 is size of ###COMPUTE###
        main_set.compute.init(full_compilation(file.substr(iter, file.find("###", iter)  - iter), shader_types::compute));
    }
    if(file.find("###TESSELATION_E###"))
    {
        iter += 19; // 19 is size of ###TESSELATION_E###
        main_set.tess_eval.init(full_compilation(file.substr(iter, file.find("###", iter)  - iter), shader_types::tese));
    }
    if(file.find("###TESSELATION_C###"))
    {
        iter += 19; // Read comment upper
        main_set.tess_control.init(full_compilation(file.substr(iter, file.find("###", iter)  - iter), shader_types::tesc));
    }
    if(file.find("###EXTRA###"))
    {
        iter += 11; //11 is size of ###EXTRA### 
        main_set.extra_info = file.substr(iter).c_str();
    }
    return main_set;
}

