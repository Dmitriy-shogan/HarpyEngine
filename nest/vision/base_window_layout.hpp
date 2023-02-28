#pragma once
#ifndef HARPY_WINDOW
#define HARPY_WINDOW
#include "..//utilities/initializations.hpp"


namespace harpy_nest{

    class hard_level_vulkan;
    class base_window_layout
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

        GLFWwindow& get_glfw_window() const;
        VkSurfaceKHR& get_VK_surface();

        base_window_layout() = default;
        
        ~base_window_layout();
};
    
}

#endif