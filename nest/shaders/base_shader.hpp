#pragma once
#ifndef HARPY_BASE_SHADER
#define HARPY_BASE_SHADER
#include "..//utilities/initializations.hpp"
#include "..//utilities/utilities.hpp"

class base_shader
{

    
public:

    static void shader_to_spiv(std::string shader_file)
    {
        
    }

    static void convert_base_shaders()
    {
        system("glsl_to_spiv.bat");
    }

    static auto create_shader_module(std::vector<char>& byte_code, VkDevice device)
    {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = byte_code.size();
        create_info.pCode = reinterpret_cast<const uint32_t*>(byte_code.data());
        
        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }
        return shader_module;
        
    }
    
    
};



#endif //HARPY_BASE_SHADER