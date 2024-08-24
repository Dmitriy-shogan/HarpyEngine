#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <iostream>
#include <nest/shader_works/glsl_shader_factory.h>
#include <nest/windowing/window.h>

#include <nest/managers/swapchain_manager.h>
#include <nest/resources/command_thread_resource.h>

#include <nest/command_commander/command_commander.h>
#include <nest/vulkan_threading/semaphor.h>
#include "nest/wrappers/data_buffer.h"
#include <nest/pipeline/graphics_pipeline.h>

#include <nest/pipeline/descriptor_factory.h>
#include <3D/uniform_buffer_objects.h>

#include <nest/texturing/texture.h>
#include <nest/texturing/texture_sampler.h>

#include <nest/windowing/input_controller.h>

#include <3D/model_loader.h>
#include <3D/camera.h>



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
void update_uniform_buffer(D3::camera& cam, wrappers::data_buffer* uniform_buffer, wrappers::swapchain* chain) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static D3::uniform_buffers::sight_ub ubo{};
    static bool get_rotated_idiot{true};

    auto currentTime = std::chrono::high_resolution_clock::now();

    if(get_rotated_idiot) {
        ubo.model = glm::mat4{1.0f};
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
        utilities::delegate forward{}, backward{};
        utilities::parameters_delegate<double*, double*> mouse_movement{};
        double camx{}, camy{};
        auto& win = resources::common_vulkan_resource::get_resource().get_main_window();
        auto center = resources::common_vulkan_resource::get_resource().get_main_window().get_screen_center();

        mouse_movement.push_back([&cam, &last_x = center.first, &last_y = center.second](double* x, double* y) mutable {
            float xoffset = *x - last_x;
            float yoffset = *y - last_y;
            last_x = *x;
            last_y = *y;

            const float sensitivity = 0.001f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            cam.get_vertical_rotation() -= yoffset;
            cam.get_hor_rotation() += xoffset;
        });
        controller.map_mouse_movement(mouse_movement);
        controller.disable_cursor();


        utilities::delegate move_delegate{};
        move_delegate.push_back([&cam](){
            cam.get_position() += glm::vec3(cam.construct_rotation_matrix() * glm::vec4(cam.get_velocity() * 0.1f, 0.f));
        });
        cam.bind_update_action(move_delegate);

        utilities::delegate forwards{}, forwards_release{};
        utilities::delegate backwards{}, backwards_release{};
        utilities::delegate left{}, right{};
        utilities::delegate left_relese{}, right_relese{};
        forwards.push_back([&cam](){
            cam.get_velocity().z = -1;
        });
        forwards_release.push_back([&cam](){
            cam.get_velocity().z = 0;
        });
        backwards.push_back([&cam](){
            cam.get_velocity().z = 1;
        });
        backwards_release.push_back([&cam](){
            cam.get_velocity().z = 0;
        });
        left.push_back([&cam](){
            cam.get_velocity().x = -1;
        });
        left_relese.push_back([&cam](){
            cam.get_velocity().x = 0;
        });
        right.push_back([&cam](){
            cam.get_velocity().x = 1;
        });
        right_relese.push_back([&cam](){
            cam.get_velocity().x = 0;
        });


        controller.map_key(GLFW_KEY_W, windowing::input_action::press, forwards);
        controller.map_key(GLFW_KEY_S, windowing::input_action::press, backwards);
        controller.map_key(GLFW_KEY_W, windowing::input_action::release, forwards_release);
        controller.map_key(GLFW_KEY_S, windowing::input_action::release, backwards_release);
        controller.map_key(GLFW_KEY_A, windowing::input_action::press, left);
        controller.map_key(GLFW_KEY_D, windowing::input_action::press, right);
        controller.map_key(GLFW_KEY_A, windowing::input_action::release, left_relese);
        controller.map_key(GLFW_KEY_D, windowing::input_action::release, right_relese);


        std::vector<wrappers::data_buffer> uniform_buffers{};
        for(auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++){
            uniform_buffers.emplace_back(wrappers::buffer_type::uniform, sizeof(D3::uniform_buffers::sight_ub));
        }

        //Initialise shaders
        shaders::glsl_shader_factory& shader_factory = shaders::glsl_shader_factory::get_singleton();
        std::unique_ptr<shaders::shader_module>
                vertex_shader{std::make_unique<shaders::shader_module>(shader_factory.create_shader_module("../external_resources/shaders/base/vertex/base.vert"))},
                fragment_shader{std::make_unique<shaders::shader_module>(shader_factory.create_shader_module("../external_resources/shaders/base/fragment/base.frag"))};

        //Initialise needed descriptors
        pipeline::descriptor_factory desc_factory{
                {{pools::descriptor_types::uniform_buffer, 2500},
                 {pools::descriptor_types::sampler, 1000}},
                5000
        };

        std::vector<VkDescriptorSetLayout> descriptor_layouts{};
        descriptor_layouts.push_back(desc_factory.allocate_descriptor_set_layout({desc_factory.get_sight_ub_binding(), desc_factory.get_image_sampler_binding()}));

        std::vector<VkDescriptorSet> descriptor_sets
                {desc_factory.allocate_descriptor_sets(descriptor_layouts)};
        auto set = descriptor_sets.front();
        descriptor_sets.push_back(set);

        //Initialize textures
        auto img = cv::imread("../external_resources/3d_objects/default/maxwell/nowhiskers.jpg");
        cv::cvtColor(img, img, cv::COLOR_YCrCb2BGR, 3, );
        utilities::image image{"../external_resources/3d_objects/default/maxwell/nowhiskers.jpg"};
        texturing::texture_sampler sampler{};
        texturing::texture texture{image};

        desc_factory.start_updating_sets()
                ->update_sight_ub(descriptor_sets.front(), uniform_buffers.front())
                ->update_sight_ub(descriptor_sets.back(), uniform_buffers.back())
                ->update_sampler(descriptor_sets.front(), texture, sampler)
                ->update_sampler(descriptor_sets.back(), texture, sampler)
                ->end_updating_descriptor_sets();

        //Create first and only swapchain
        managers::swapchain_manager& swapchain_man = managers::swapchain_manager::get_singleton();
        swapchain_man.add_swapchain();

        //Initialise pipeline
        pipeline::graphics_pipeline_ci ci{

                .modules = shaders::shader_set{
                        .vertex = std::move(vertex_shader),
                        .fragment = std::move(fragment_shader)
                },

                .swapchain = &swapchain_man.get_swapchain(),
                .descriptor_layouts = &descriptor_layouts
        };


        swapchain_man.make_available(*ci.swapchain);

        auto& chain = swapchain_man.get_swapchain();

        pipeline::graphics_pipeline pipe{&ci};

        //Create and initialise threading objects
        threading::fence fence{};
        threading::semaphore swapchain_sem{}, present_sem{};

        //Initialising queues
        managers::queue_manager universal_manager{
                resources::common_vulkan_resource::get_resource().get_main_family_queue()};
        //Only for нищие дебилычи с ноутом и встроенкой (меня)
        VkQueue present_queue = universal_manager.get_queue();

        wrappers::queue_family transfer_queue = [&]() mutable {
            auto& res = resources::common_vulkan_resource::get_resource().get_all_family_queues();
            for(auto& i : res)
                if(i.get_type() == wrappers::queue_type::transfer)
                    return i;
        }();

        managers::queue_manager transfer_manager{transfer_queue};

        //Create and initialise thread resource
        std::unique_ptr<resources::command_thread_resource>
                thread_res{new resources::command_thread_resource
                                   {
                                           .queue = universal_manager.get_queue(),
                                           .com_pool = std::make_unique<pools::command_pool>
                                                   (resources::common_vulkan_resource::get_resource().get_main_family_queue())
                                   }
        };

        //Create and initialise thread resource
        std::unique_ptr<resources::command_thread_resource>
                thread_res2{new resources::command_thread_resource
                                    {
                                            .queue = transfer_manager.get_queue(),
                                            .com_pool = std::make_unique<pools::command_pool>
                                                    (transfer_queue)
                                    }
        };



        D3::model_loader loader{std::move(thread_res2)};
        auto md = loader.load_model("../external_resources/3d_objects/default/viking_room/viking_room.obj");


        //Camera fun





        //Create and initialise command commander
        command_commander commander{};
        commander.bind_thread_res(std::move(thread_res));

        VkRect2D scissors{
                .offset = {0, 0},
                .extent = chain.get_extent()
        };

        VkViewport viewport
                {
                        .x = 0.0f,
                        .y = 0.0f,
                        .width = static_cast<float> (chain.get_extent().width),
                        .height = static_cast<float> (chain.get_extent().height),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f
                };

        //Load the object
        commander.reset_pool()
                ->start_recording()
                ->load_into_texture(texture, image)
                ->end_recording()();
        commander.submit();



        uint32_t image_index{};

        //Main loop for now
        while(!glfwWindowShouldClose(resources::common_vulkan_resource::get_resource().get_main_window()))
        {
            glfwPollEvents();
            cam.update();

            fence.wait_and_reset_fence();
            update_uniform_buffer(cam, &uniform_buffers[image_index], &chain);

            image_index = ci.swapchain->acquire_vk_image_index(&swapchain_sem, nullptr);

            auto render_delegate = commander.reset_pool()
                    ->start_recording()
                    ->start_render_pass(chain, image_index)
                    ->bind_pipeline(pipe)
                    ->set_scissors(scissors)
                    ->set_viewport(viewport)
                    ->bind_vertex_buffers(md->get_mesh().get_vertices())
                    ->bind_index_buffer(md->get_mesh().get_indices())
                    ->bind_descriptor_sets(descriptor_sets[image_index], pipe)
                    ->draw_indexed(md->get_mesh().get_indices().get_size() / sizeof(uint32_t))
                    ->end_render_pass()
                    ->end_recording();

            render_delegate();

            commander.submit(&fence, &present_sem, &swapchain_sem);

            show_on_screen(chain, present_sem, image_index, present_queue);
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

