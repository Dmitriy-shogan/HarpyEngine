#pragma once
#ifndef HARPY_WINDOW_LAYOUT
#define HARPY_WINDOW_LAYOUT
//#include <utilities/initializations.h>
#include "../spinal_cord/vulkan_spinal_cord.h"
#include <renderer_context/renderer_context.h>
#include <memory>
using namespace harpy::nest;
namespace harpy::nest{

    //class hard_level_vulkan;
    struct base_window_layout
    {
        //friend hard_level_vulkan;
    	nest::renderer_context * r_context;
        //std::shared_ptr<vulkan_spinal_cord> cord;
        GLFWwindow* glfw_window{nullptr};
        VkSurfaceKHR surface {nullptr};
        bool resized {false};

        void init_window();
        void init_surface();

       
        
    public:

        GLFWwindow*& get_glfw_window();
        VkSurfaceKHR& get_VK_surface();
        void init(renderer_context * r_context);
        

        base_window_layout() = default;
        
        ~base_window_layout();

        static void base_framebuffer_resize(GLFWwindow* window, int width, int height);
        bool& get_resize();
    };
    
    
}

#endif // HARPY_WINDOW_LAYOUT
