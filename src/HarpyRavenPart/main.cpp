#define GLM_FORCE_RADIANS
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
#include <nest/wrappers/buffers/data_buffer.h>
#include <nest/pipeline/graphics_pipeline.h>

#include <threading/thread_pool.h>

#include <nest/pipeline/descriptors/descriptor_allocator.h>
#include <nest/pipeline/descriptors/descriptor_factory.h>
#include <3D/uniform_buffer_objects.h>


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
    presentInfo.pWaitSemaphores = &sem.get_vk_semaphore();

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain.get_vk_swapchain();

    presentInfo.pImageIndices = &image_number;

    HARPY_VK_CHECK(vkQueuePresentKHR(queue, &presentInfo));
}
void update_uniform_buffer(wrappers::data_buffer* uniform_buffer, wrappers::swapchain* chain) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    static D3::uniform_buffers::sight_ub ubo{.view =
    glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f))};

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(90.0f), chain->get_extent().width / (float) chain->get_extent().height, 0.1f, 100.0f);
    ubo.proj[1][1] *= -1;

    memcpy(uniform_buffer->get_mapped_ptr(), &ubo, sizeof(ubo));
    uniform_buffer->unmap_ptr();
}


/*
 * TODO: make ALL constructors include a VkDevice as argument with default value = common_vulkan_resource::get_resource().get_main_device() and rewrite all init functions into constructor
 */

int main()
{
    initializations::init_harpy();
    try {
        //Initialise buffers
        wrappers::data_buffer vertex_buffer{wrappers::buffer_type::vertex, vertices.size()};
        wrappers::data_buffer index_buffer{wrappers::buffer_type::indice, indices.size()};
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
        pools::pool_size_desc desc{pools::descriptor_types::uniform_buffer, 2500};
        pipeline::descriptors::descriptor_allocator allocator{{desc}};
        pipeline::descriptors::descriptor_factory desc_factory{allocator};
        std::vector<VkDescriptorSetLayout> descriptor_layouts{desc_factory.create_descriptor_layout(pipeline::descriptors::descriptor_factory::sight_ub_binding())};
        auto descriptor_sets = desc_factory.allocate_descriptor_set(descriptor_layouts.front(), MAX_FRAMES_IN_FLIGHT);

        desc_factory.update_descriptor_sets_sight_ub(desc_factory.populate_sight_ub(uniform_buffers.front()), descriptor_sets);
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
        VkQueue present_queue = universal_manager.get_queue();

        //Create and initialise thread resource
        std::unique_ptr<resources::command_thread_resource>
                thread_res{new resources::command_thread_resource
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
        commander.submit();

        uint32_t image_index{};

        //Main loop for now
        while(!glfwWindowShouldClose(resources::common_vulkan_resource::get_resource().get_main_window()))
        {
            glfwPollEvents();

            fence.wait_and_reset_fence();
            update_uniform_buffer(&uniform_buffers[image_index], &chain);

            image_index = ci.swapchain->acquire_vk_image_index(&swapchain_sem, nullptr);

            auto render_delegate = commander.reset_pool()
                    ->start_recording()
                    ->start_render_pass(chain, image_index)
                    ->bind_pipeline(pipe)
                    ->set_scissors(scissors)
                    ->set_viewport(viewport)
                    ->bind_vertex_buffers(vertex_buffer)
                    ->bind_index_buffer(index_buffer)
                    ->bind_descriptor_sets(descriptor_sets[image_index], pipe)
                    ->draw_indexed(indices.size())
                    ->end_render_pass()
                    ->end_recording();

            render_delegate();

            commander.submit(&fence, &present_sem, &swapchain_sem);

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

