#include "..//base_window.hpp"

void harpy_nest::base_window::framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<harpy_nest::base_window*>(glfwGetWindowUserPointer(window));
    app->do_resize = true;
}

void harpy_nest::base_window::init_window()
{


    glfw_window = glfwCreateWindow(800, 600, "Cool title", nullptr, nullptr);

    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_resize_callback);
}

void harpy_nest::base_window::init_surface(VkInstance& instance)
{
    if (glfwCreateWindowSurface(instance, glfw_window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void harpy_nest::base_window::init_all(VkInstance& instance)
{
    init_window();
    init_surface(instance);
}

harpy_nest::base_window::~base_window()
{
    glfwDestroyWindow(glfw_window);
}
