#pragma once
#ifndef HARPY_WINDOW
#define HARPY_WINDOW
#include "..//utilities/initializations.hpp"


namespace harpy_nest{

    class hard_level_vulkan;
    class base_window
    {
        friend hard_level_vulkan;
        GLFWwindow* glfw_window{nullptr};
        VkSurfaceKHR surface {nullptr};
        bool do_resize {false};
        
        static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);

        void init_window();
        void init_surface(VkInstance& instance);

        void init_all(VkInstance& instance);
        
    public:

        base_window() = default;
        
        ~base_window();
};
    
}

#endif