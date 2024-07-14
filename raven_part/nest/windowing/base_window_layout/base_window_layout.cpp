#include "../base_window_layout.h"

#include <renderer_context/renderer_context.h>

void harpy::nest::base_window_layout::init_window()
{
    glfw_window = glfwCreateWindow(800, 600, "Cool title", nullptr, nullptr);

    glfwMakeContextCurrent(glfw_window);
    glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(glfw_window, this);
    glfwSetFramebufferSizeCallback(glfw_window, base_framebuffer_resize);


}

void harpy::nest::base_window_layout::init_surface()
{
    if (glfwCreateWindowSurface(r_context->spinal_cord->instance, glfw_window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void harpy::nest::base_window_layout::init(renderer_context * r_context)
{
	//this->cord = cord;
	std::cout<<"aaaa"<<std::endl;
	this->r_context = r_context;
	std::cout<<"initialising base_window_layout started"<<std::endl;
    init_window();
    std::cout<<"init init_window succ"<<std::endl;
    init_surface();
    std::cout<<"init init_surface succ"<<std::endl;
}

GLFWwindow*& harpy::nest::base_window_layout::get_glfw_window()
{return glfw_window;}

VkSurfaceKHR& harpy::nest::base_window_layout::get_VK_surface()
{
    return surface;
}

harpy::nest::base_window_layout::~base_window_layout()
{
	if (!r_context) return;
	if(surface)
	vkDestroySurfaceKHR(r_context->spinal_cord->instance, surface, nullptr);
	if(glfw_window)
	glfwDestroyWindow(glfw_window);
}

void harpy::nest::base_window_layout::base_framebuffer_resize(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<base_window_layout*>(glfwGetWindowUserPointer(window));
    app->resized = true;
}

bool& harpy::nest::base_window_layout::get_resize()
{ return resized; }
