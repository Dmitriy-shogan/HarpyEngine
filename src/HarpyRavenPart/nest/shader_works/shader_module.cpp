#include <nest/shader_works/shader_module.h>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/shader_works/glsl_shader_factory.h>

using resource = harpy::nest::resources::common_vulkan_resource;


harpy::nest::shaders::shader_module::shader_module(const spirv_compilation_result& data, shader_types type, VkDevice* device) : type(type), device(device)
{
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = data.size() * sizeof(uint32_t);
    ci.pCode = data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

harpy::nest::shaders::shader_module::shader_module(const std::string& shader_assembly, shader_types type, VkDevice* device) : type(type), device(device)
{
    auto shader_data = glsl_shader_factory::get_singleton().build_assembly(shader_assembly);
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = shader_data.size() * sizeof(uint32_t);
    ci.pCode = shader_data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

harpy::nest::shaders::shader_module::shader_module(spirv_compilation_result&& data, shader_types type, VkDevice* device) : type(type), device(device)
{
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = data.size() * sizeof(uint32_t);
    ci.pCode = data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

harpy::nest::shaders::shader_module::shader_module(std::string&& shader_assembly, shader_types type, VkDevice* device) : type(type), device(device)
{
    auto shader_data = glsl_shader_factory::get_singleton().build_assembly(shader_assembly);
    VkShaderModuleCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = shader_data.size() * sizeof(uint32_t);
    ci.pCode = shader_data.data();

    vkCreateShaderModule(resource::get_resource(), &ci, nullptr, &shader);
}

harpy::nest::shaders::shader_module::shader_module(const shader_module& shader)
{
    type = shader.type;
}

harpy::nest::shaders::shader_module::shader_module(shader_module&& shader) noexcept
{
    this->shader = shader.shader;
    shader.shader = nullptr;
    this->type = shader.type;
}

harpy::nest::shaders::shader_module& harpy::nest::shaders::shader_module::operator=(const shader_module& shader) 
{
    type = shader.type;
    return *this;
}

harpy::nest::shaders::shader_module& harpy::nest::shaders::shader_module::operator=(shader_module&& shader) noexcept
{
    this->shader = shader.shader;
    shader.shader = nullptr;
    this->type = shader.type;
    return *this;
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


