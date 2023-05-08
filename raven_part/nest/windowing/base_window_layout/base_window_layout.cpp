#include "..//base_window_layout.h"


void harpy::nest::windowing::base_window_layout::init_window()
{
    glfw_window = glfwCreateWindow(800, 600, "Cool title", nullptr, nullptr);
    
    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, base_framebuffer_resize);
}

void harpy::nest::windowing::base_window_layout::init_surface(VkInstance& instance)
{
    if (glfwCreateWindowSurface(instance, glfw_window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void harpy::nest::windowing::base_window_layout::init_all(VkInstance& instance)
{
    init_window();
    init_surface(instance);
}

GLFWwindow*& harpy::nest::windowing::base_window_layout::get_glfw_window() 
{return glfw_window;}

VkSurfaceKHR& harpy::nest::windowing::base_window_layout::get_VK_surface()
{
    return surface;
}

harpy::nest::windowing::base_window_layout::~base_window_layout()
{
    glfwDestroyWindow(glfw_window);
}

void harpy::nest::windowing::base_window_layout::base_framebuffer_resize(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<base_window_layout*>(glfwGetWindowUserPointer(window));
    app->resized = true;
}

bool& harpy::nest::windowing::base_window_layout::get_resize()
{ return resized; }
