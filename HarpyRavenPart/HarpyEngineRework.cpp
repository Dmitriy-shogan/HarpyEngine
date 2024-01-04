#include <iostream>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/shader_works/glsl_shader_factory.h>
#include <nest/windowing/window.h>

#include "nest/managers/swapchain_manager.h"
#include "nest/resources/record_cis_resource.h"
#include <nest/resources/command_thread_resource.h>

#include "nest/command_commander/command_commander.h"
#include "nest/wrappers/buffers/data_buffer.h"

using namespace harpy;
using namespace nest;

std::vector<wrappers::vertex> vertices = {
    {{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f, 1.0f}}
};
std::vector<uint32_t> indices = {
    0, 1, 2, 2, 3, 0
};

/*
 * TODO: make ALL inits include a VkDevice as argument with default value = common_vulkan_resource::get_resource().get_main_device()
 * TODO: make in ALL classes with init a ptr to VkDevice and in init initialize it with given value 
 */

int main(int argc, char* argv[])
{
    try {
        initializations::init_harpy();
        wrappers::render_pass render_pass{};
        render_pass.init();

       wrappers::data_buffer vertex_buffer{};
        vertex_buffer.init(vertices.size(), wrappers::buffer_type::vertex);

        wrappers::data_buffer index_buffer{};
        index_buffer.init(indices.size(), wrappers::buffer_type::indice);
        
        managers::swapchain_manager swapchains{};
        swapchains.init();
        
        resources::records_cis records{new resources::record_cis_resource(render_pass)};
        
        std::shared_ptr<resources::command_thread_resource> thread_res{new resources::command_thread_resource};
        thread_res->queue_type = wrappers::universal;
        thread_res->queue = std::make_shared<VkQueue>(resources::common_vulkan_resource::get_resource().get_main_family_queue().get_vk_queue(0));
        thread_res->render_pass = std::make_shared<wrappers::render_pass>(render_pass);

        command_commander commander{};
        commander.bind_records_cis(records);
        commander.bind_thread_res(thread_res);
        
        auto deleg = commander.start_recording()->load_into_buffer(
            vertex_buffer.get_vk_buffer(),
            vertices.data(),
            vertex_buffer.get_size())->
        load_into_buffer(index_buffer.get_vk_buffer(),
            indices.data(),
            index_buffer.get_size())->
        end_recording();
        deleg();
        
        
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
