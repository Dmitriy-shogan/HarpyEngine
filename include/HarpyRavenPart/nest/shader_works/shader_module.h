#pragma once
#ifndef HARPY_NEST_SHADER_MODULE
#define HARPY_NEST_SHADER_MODULE
#include <any>
#include <string>
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::shaders
{
    enum class shader_types : uint8_t
    {
        vertex = 0,
        fragment,
        compute,
        geometry,
        tesc,
        tese,

        he_first = vertex,
        he_bm = 0,
        he_count = 6,
        
    };
    
    class shader_module
    {
        VkDevice* device{};
        VkShaderModule shader{};
        shader_types type{};
        shader_module() = default;

        friend struct shader_set;
        friend class glsl_shader_factory;
    public:
        
        shader_module(const spirv_compilation_result& data, shader_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        shader_module(const std::string& shader_assembly, shader_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        shader_module(spirv_compilation_result&& data, shader_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        shader_module(std::string&& shader_assembly, shader_types type, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        shader_module(const shader_module& shader);
        shader_module(shader_module&& shader) noexcept;

        shader_module& operator=(const shader_module& shader);
        shader_module& operator=(shader_module&& shader) noexcept;
        
        VkShaderModule& get_shader();
        shader_types get_type();
        operator VkShaderModule&();

        ~shader_module();
    };

    struct shader_set
    {
        std::unique_ptr<shader_module> vertex{nullptr};
        std::unique_ptr<shader_module> fragment{nullptr};
        std::unique_ptr<shader_module> tess_control{nullptr};
        std::unique_ptr<shader_module> tess_eval{nullptr};
        std::unique_ptr<shader_module> geometry{nullptr};
        std::any extra_info{};
    };
}

#endif//HARPY_NEST_SHADER_MODULE