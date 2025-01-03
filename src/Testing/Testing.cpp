#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include <nest/windowing/input_controller.h>
#include <3D/renderer.h>
#include <nest/shader_works/glsl_shader_factory.h>
#include <nest/texturing/texture_sampler.h>

#include "3D/model_loader.h"


using namespace harpy;
using namespace nest;

void show_on_screen(wrappers::swapchain& swapchain, threading::semaphore& sem, uint32_t& image_number, VkQueue& queue)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &sem.get_vk_semaphore();

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.get_vk_swapchain();

    presentInfo.pImageIndices = &image_number;

    HARPY_VK_CHECK(vkQueuePresentKHR(queue, &presentInfo));
}
void update_uniform_buffer(D3::camera& cam, wrappers::data_buffer* uniform_buffer) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static D3::uniform_buffers::sight_ub ubo{};
    static bool get_rotated_idiot{true};

    auto currentTime = std::chrono::high_resolution_clock::now();

    if(get_rotated_idiot) {
        ubo.view_proj = glm::mat4{1.0f};
        //ubo.model = glm::rotate(ubo.model, glm::radians(90.0f), glm::vec3(1.0f, 1.0f, 0.0f));
        get_rotated_idiot = false;
    }
/*
    ubo.model = glm::rotate(ubo.model, glm::radians(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
*/


    ubo.view = cam.construct_view_matrix();
    ubo.proj = cam.construct_perspective_matrix(16.f/9.f);

    memcpy(uniform_buffer->get_mapped_ptr(), &ubo, sizeof(ubo));
    uniform_buffer->unmap_ptr();
}


/*
 * TODO: make ALL constructors include a VkDevice as argument with default value = common_vulkan_resource::get_resource().get_main_device() and rewrite all init functions into constructor
 */

int main()
{
    windowing::window_create_info ci{};
    ci.height = 900;
    ci.width = 1600;
    ci.refresh_rate = 145;
    ci.is_focused_on_show = 1;
    ci.is_resizable = false;
    ci.is_decorated = true;
    ci.name = "BEST WINDOW EVER EVER!!!";

    initializations::init_harpy(ci);

    try {
        D3::camera cam{15, 10, -25};
        windowing::input_controller controller{};

        D3::camera::set_standard_mouse_movement(cam, 0.001f, controller);
        D3::camera::set_standard_movement(cam, 0.1f, controller);

        D3::renderer renderer{};
        renderer.add_render_target("../external_resources/3d_objects/default/maxwell/maxwell.fbx", "le_funny_cat");

        utilities::image image{"../external_resources/3d_objects/default/maxwell/nowhiskers.jpg"};
        texturing::texture_sampler sampler{};
        texturing::texture texture{"default", image};

        while(!glfwWindowShouldClose(resources::common_vulkan_resource::get_resource().get_main_window()))
        {
            glfwPollEvents();
            cam.update();

            renderer.render()->
            show_on_screen();
        }
    }
    catch (utilities::harpy_little_error& error)
    {
        utilities::logger::get_logger().log(error);
        utilities::logger::get_logger().show_last_log();
    }
    catch(std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }


    return 0;
}

