#pragma once
#ifndef HARPY_SHADERS_MODULE
#define HARPY_SHADERS_MODULE
#include <utilities/harpy_little_error.h>
#include <utilities/utilities.h>

namespace harpy::nest::shaders
{
    enum shader_type
        {
            none = 0,
            vertex = 1, //vert
            fragment = 1 << 1, //frag
            geometry = 1 << 2, //geom
            compute = 1 << 3, //comp
            tess_control = 1 << 4, //tesc
            tess_eval = 1 << 5, //tese
        };
    class shader_module
    {
        VkDevice& device;

        std::string filepath;
        VkShaderModule module{nullptr};
        shader_type type;
        
    public:
        shader_module(VkDevice& device) : device(device), type(none){}

        void init(std::string filepath)
        {
            this->filepath = filepath;
            filepath.replace(filepath.find('.'), 5, ".spv");
            std::vector<char> byte_code;
            try
            {
                 byte_code = utilities::read_file(filepath);
            }
            catch(std::runtime_error&)
            {
                throw utilities::harpy_little_error("Can't find correct spv shader file");
            }

            switch(this->filepath.back())
            {
            case 't':
                {
                    type = vertex;
                    break;
                }
            case 'g':
                {
                    type = fragment;
                    break;
                }
            case 'm':
                {
                    type = geometry;
                    break;
                }
            case 'p':
                {
                    type = compute;
                    break;
                }
            case 'c':
                {
                    type = tess_control;
                    break;
                }
            case 'e':
                {
                    type = tess_eval;
                    break;
                }
            default: throw utilities::harpy_little_error("Trying to create shader module from non-shader file");
            }
            VkShaderModuleCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            create_info.codeSize = byte_code.size();
            create_info.pCode = reinterpret_cast<const uint32_t*>(byte_code.data());
            
            if (vkCreateShaderModule(device, &create_info, nullptr, &module) != VK_SUCCESS) {
                throw std::runtime_error("failed to create shader module!");
            }
        }

        VkShaderModule& get_vk_shader_module(){return module;}
        shader_type get_shader_type(){return type;}

        operator VkShaderModule&(){return module;}
        
        ~shader_module()
        {
            vkDestroyShaderModule(device, module, nullptr);
        }
    
    };
}

#endif //HARPY_SHADERS_MODULE
