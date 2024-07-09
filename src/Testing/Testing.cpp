#include <iostream>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/shader_works/glsl_shader_factory.h>
#include <nest/windowing/window.h>

#include <nest/managers/swapchain_manager.h>
#include <nest/resources/command_thread_resource.h>

#include <nest/command_commander/command_commander.h>
#include <nest/vulkan_threading/semaphor.h>
#include <nest/wrappers/buffers/data_buffer.h>
#include <nest/pipeline/graphics_pipeline.h>

#include <threading/thread_pool.h>

#include <nest/pipeline/descriptors/descriptor_allocator.h>
#include <nest/pipeline/descriptors/descriptor_factory.h>


using namespace harpy;
using namespace nest;

std::vector<wrappers::vertex> vertices = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
};

std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
};

void show_on_screen(wrappers::swapchain& swapchain, threading::semaphore& sem, uint32_t& image_number, VkQueue& queue)
{
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &sem.get_semaphore();

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.get_vk_swapchain();

    presentInfo.pImageIndices = &image_number;

    HARPY_VK_CHECK(vkQueuePresentKHR(queue, &presentInfo));
}


/*
 * TODO: make ALL constructors include a VkDevice as argument with default value = common_vulkan_resource::get_resource().get_main_device() and rewrite all init functions into constructor
 */

int main()
{
    initializations::init_harpy();
    try {
        //Initialise vertex and index buffers
        wrappers::data_buffer vertex_buffer{wrappers::buffer_type::vertex, vertices.size()};
        wrappers::data_buffer index_buffer{wrappers::buffer_type::indice, indices.size()};
        //Initialise shaders
        shaders::glsl_shader_factory& shader_factory = shaders::glsl_shader_factory::get_singleton();
        std::unique_ptr<shaders::shader_module>
                vertex_shader{std::make_unique<shaders::shader_module>(shader_factory.create_shader_module("../shaders/base/vertex/base.vert"))},
                fragment_shader{std::make_unique<shaders::shader_module>(shader_factory.create_shader_module("../shaders/base/fragment/base.frag"))};

        //Create first and only swapchain
        managers::swapchain_manager& swapchain_man = managers::swapchain_manager::get_singleton();
        swapchain_man.add_swapchain();

        //Initialise pipeline
        pipeline::graphics_pipeline_ci ci{
                .modules = shaders::shader_set{
                        .vertex = std::move(vertex_shader),
                        .fragment = std::move(fragment_shader)},

                .swapchain = &swapchain_man.get_swapchain()
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

        VkQueue present_queue = universal_manager.get_queue();
        //Create and initialise thread resource
        std::unique_ptr<resources::command_thread_resource> thread_res{new resources::command_thread_resource
                                                                               {
                                                                                       .queue = universal_manager.get_queue(),
                                                                                       .com_pool = std::make_unique<pools::command_pool>
                                                                                               (resources::common_vulkan_resource::get_resource().get_main_family_queue())
                                                                               }
        };

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

        //Loading data into buffers
        auto deleg = commander.start_recording()->load_vertex_index_buffers(
                vertices.data(),
                vertices.size() * sizeof(wrappers::vertex),
                indices,
                vertex_buffer,
                index_buffer)->end_recording();

        deleg();
        commander.submit(fence, present_sem, threading::semaphore{});

        uint32_t image_index{};

        //Main loop for now
        while(!glfwWindowShouldClose(resources::common_vulkan_resource::get_resource().get_main_window()))
        {
            glfwPollEvents();

            fence.wait_and_reset_fence();

            image_index = ci.swapchain->acquire_vk_image_index(swapchain_sem, nullptr);

            auto render_delegate = commander.reset_pool()
                    ->start_recording()
                    ->start_render_pass(chain, image_index)
                    ->bind_pipeline(pipe)
                    ->set_scissors(scissors)
                    ->set_viewport(viewport)
                    ->bind_vertex_buffers(vertex_buffer)
                    ->bind_index_buffer(index_buffer)
                    ->draw_indexed(indices.size())
                    ->end_render_pass()
                    ->end_recording();

            render_delegate();

            commander.submit(fence, present_sem, swapchain_sem);

            show_on_screen(chain, present_sem, image_index, present_queue);
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


    return 0;
}

