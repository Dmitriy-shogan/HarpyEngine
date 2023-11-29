#pragma once
#ifndef HARPY_NATIVE_WINDOWING
#define HARPY_NATIVE_WINDOWING
#include <GLFW/glfw3.h>
#include <string>
#include <glm/vec2.hpp>


namespace harpy::nest::windowing
{
    struct window_create_info
    {
        int width;
        int height;
        bool is_fullscreen;
        bool is_borderless;
        std::string name;
    };
    class window
    {
        GLFWwindow* win{};
        GLFWvidmode* video_mode{};
        window_create_info ci{};

        void init();
        
    public:
        window(window_create_info ci);

        //TODO: fix underlaying bullshit
        window(window&) = delete;
        window& operator=(window&) = delete;

        window(window&&) noexcept;
        window& operator=(window&&) noexcept;

        GLFWwindow*& get_glfw_window();
        operator GLFWwindow*&();

        std::pair<int, int> get_size();
        int get_width();
        int get_height();

        void set_size(int width, int height);
        void set_size(std::pair<int, int> size);
        void set_size(glm::vec2 size);
        

        ~window();
    };
    
}



#endif