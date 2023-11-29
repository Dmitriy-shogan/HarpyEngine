#include <iostream>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/initializations/inititalizations.h>
#include <nest/shader_works/glsl_shader_factory.h>
#include <nest/windowing/window.h>

using namespace harpy;
using namespace nest;

/*
 * TODO: make ALL inits include a VkDevice as argument with default value = common_vulkan_resource::get_resource().get_main_device()
 * TODO: make in ALL classes with init a ptr to VkDevice and in init initialize it with given value 
 */

int main(int argc, char* argv[])
{
    try{
        initializations::init_harpy();
        
        //Main loop for now
        while(!glfwWindowShouldClose(resources::common_vulkan_resource::get_resource().get_main_window()))
        {
            glfwPollEvents();
        }
        
    }
    catch (utilities::error_handling::harpy_little_error& error)
    {
        utilities::error_handling::logger::get_logger().log(error);
        utilities::error_handling::logger::get_logger().show_last_log();
    }
    catch(std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }

    resources::common_vulkan_resource::get_resource().clear_resource();
    return 0;
}
