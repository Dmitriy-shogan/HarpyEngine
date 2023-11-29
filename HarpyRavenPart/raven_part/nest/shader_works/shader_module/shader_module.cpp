#include "shader_module.h"
#include <utilities/util/util.h>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/shader_works/glsl_shader_factory.h>

using resource = harpy::nest::resources::common_vulkan_resource;

harpy::nest::shaders::shader_module::shader_module(shader_types type) : type(type)
{
}

void harpy::nest::shaders::shader_module::init(const std::string& shader_assembly)
{
    auto shader_data = glsl_shader_factory::get_singleton().build_assembly(shader_assembly);
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = shader_data.size() * sizeof(uint32_t);
    ci.pCode = shader_data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

void harpy::nest::shaders::shader_module::init(const spirv_compilation_result& data)
{
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = data.size() * sizeof(uint32_t);
    ci.pCode = data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

VkShaderModule& harpy::nest::shaders::shader_module::get_shader()
{return shader;}

harpy::nest::shaders::shader_types harpy::nest::shaders::shader_module::get_type()
{
    return type;
}

harpy::nest::shaders::shader_module::operator VkShaderModule_T*&()
{return shader;}

harpy::nest::shaders::shader_module::~shader_module()
{
 vkDestroyShaderModule(resource::get_resource(), shader, nullptr);           
}


