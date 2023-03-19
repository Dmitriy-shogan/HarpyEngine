#pragma once
#ifndef HARPY_WINDOW_LAYOUT
#define HARPY_WINDOW_LAYOUT
#include <utilities/initializations.h>


namespace harpy_nest{

    class hard_level_vulkan;
    class base_window_layout
    {
        friend hard_level_vulkan;
        GLFWwindow* glfw_window{nullptr};
        VkSurfaceKHR surface {nullptr};
        bool resized {false};

        void init_window();
        void init_surface(VkInstance& instance);

        void init_all(VkInstance& instance);
        
    public:

        GLFWwindow*& get_glfw_window();
        VkSurfaceKHR& get_VK_surface();

        base_window_layout() = default;
        
        ~base_window_layout();

        static void base_framebuffer_resize(GLFWwindow* window, int width, int height);
        bool& get_resize();
    };
    
    
}

#endif // HARPY_WINDOW_LAYOUT