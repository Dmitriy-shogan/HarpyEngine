#include "..//base_window_layout.h"


void harpy_nest::base_window_layout::init_window()
{
    glfw_window = glfwCreateWindow(800, 600, "Cool title", nullptr, nullptr);
    
    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, base_framebuffer_resize);
}

void harpy_nest::base_window_layout::init_surface(VkInstance& instance)
{
    if (glfwCreateWindowSurface(instance, glfw_window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void harpy_nest::base_window_layout::init_all(VkInstance& instance)
{
    init_window();
    init_surface(instance);
}

GLFWwindow*& harpy_nest::base_window_layout::get_glfw_window() 
{return glfw_window;}

VkSurfaceKHR& harpy_nest::base_window_layout::get_VK_surface()
{
    return surface;
}

harpy_nest::base_window_layout::~base_window_layout()
{
    glfwDestroyWindow(glfw_window);
}
