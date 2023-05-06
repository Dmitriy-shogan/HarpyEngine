#pragma once
#ifndef HARPY_BASE_SHADER
#define HARPY_BASE_SHADER
#include <shaders/shader_module.h>

namespace harpy::nest::shaders
{
    class base_shader
    {
        VkDevice& device; 
        shader_module shader{device};

    
    public:

        base_shader(VkDevice& device) : device(device) {}

        void init(std::string filepath)
        {
            shader.init(std::move(filepath));
        }

        //Legacy code, if we can say so.
        static void convert_base_shaders()
        {
            system("glsl_to_spiv.bat");
        }
    
    
    };
}

#endif //HARPY_BASE_SHADER