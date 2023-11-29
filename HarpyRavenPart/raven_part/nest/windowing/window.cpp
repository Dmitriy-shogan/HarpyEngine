#include "window.h"

#include "utilities/logger/harpy_little_error.h"

void harpy::nest::windowing::window::init()
{
    if (ci.is_fullscreen)
    {
        win = glfwCreateWindow(ci.width, ci.height, ci.name.c_str(), glfwGetPrimaryMonitor(), nullptr);
        return;
    }
    if(ci.is_borderless)
    {
        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
        win = glfwCreateWindow(video_mode->width, video_mode->height, ci.name.c_str(), nullptr, nullptr);
        return;
    }
    win = glfwCreateWindow(ci.width, ci.height, ci.name.c_str(), nullptr, nullptr);
}

harpy::nest::windowing::window::window(window_create_info ci)
{
    this->ci = ci;
    video_mode = const_cast<GLFWvidmode*>(glfwGetVideoMode(glfwGetPrimaryMonitor()));
    if (!win) init();
}

harpy::nest::windowing::window::window(window&& window) noexcept
{
    ci = window.ci;
    
    if (!window.win)
        init();
    else
    {
        win = window.win;
        window.win = nullptr;
    }
    video_mode = window.video_mode;
    
}

harpy::nest::windowing::window& harpy::nest::windowing::window::operator=(window&& window) noexcept
{
    win = window.win;
    video_mode = window.video_mode;
    window.win = nullptr;
    window.video_mode = nullptr;
    ci = window.ci;
    return *this;
}

GLFWwindow*& harpy::nest::windowing::window::get_glfw_window()
{return win;}

harpy::nest::windowing::window::operator GLFWwindow*&()
{return win;}

std::pair<int, int> harpy::nest::windowing::window::get_size()
{
    return {ci.width, ci.height};
}

int harpy::nest::windowing::window::get_width()
{
    return ci.width;
}

int harpy::nest::windowing::window::get_height()
{
    return ci.height;
}

void harpy::nest::windowing::window::set_size(int width, int height)
{
    ci.width = width;
    ci.width = height;
}

void harpy::nest::windowing::window::set_size(std::pair<int, int> size)
{
    ci.width = size.first;
    ci.height = size.second;
}

void harpy::nest::windowing::window::set_size(glm::vec2 size)
{
    ci.width = size.x;
    ci.height = size.y;
}

harpy::nest::windowing::window::~window()
{
    if(win) glfwDestroyWindow(win);
}
