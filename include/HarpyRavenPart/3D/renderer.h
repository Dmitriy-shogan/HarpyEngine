#pragma once
#ifndef HARPY_3D_RENDERER
#define HARPY_3D_RENDERER
#include <nest/command_commander/command_commander.h>

#include <3D/mesh.h>
#include <3D/model.h>
#include <3D/camera.h>

#include <nest/pools/descriptor_pool.h>

namespace harpy::D3
{
    //TODO: rethink renderer
    /* Some thoughts:
     * We can pass only two matrices: model and cam_view, while proj can be somewhat static i think
     *
     *
     */
    class renderer
    {

        nest::command_commander commander{};

        nest::wrappers::swapchain swapchain{};

        std::vector<nest::managers::queue_manager> queue_managers{};

        nest::pools::descriptor_pool descriptor_factory{};

        std::vector<std::unique_ptr<nest::resources::command_thread_resource>> thread_resources{};
        std::unique_ptr<nest::resources::command_thread_resource>* transfer_thread_resource{nullptr};

        std::unordered_map<sz::string, nest::pipeline::graphics_pipeline> pipelines{};

        camera cam{0, 0, 0};


        struct frame_data {
            unsigned short frame_index{};
            nest::threading::fence render_fence{true};

            nest::threading::semaphore render_finish_sem{};
            nest::threading::semaphore present_sem{};

            nest::wrappers::data_buffer sight_uniform_buffer{nest::wrappers::buffer_type::uniform, sizeof(uniform_buffers::sight_ub)};

            uniform_buffers::sight_ub sight_uniform{};

            VkDescriptorSet global_descriptor_set{};
        };

        std::vector<frame_data> frames_data{2}; //Todo: unhardcode

       std::vector<std::unique_ptr<model>> render_targets{};

        nest::threading::semaphore semaphore{};

        VkDescriptorSetLayout global_layout{};

        VkRect2D scissors{
            .offset = {0, 0},
            .extent = swapchain.get_extent()
    };
        VkViewport viewport
                {
                    .x = 0.0f,
                    .y = 0.0f,
                    .width = static_cast<float> (swapchain.get_extent().width),
                    .height = static_cast<float> (swapchain.get_extent().height),
                    .minDepth = 0.0f,
                    .maxDepth = 1.0f
            };

        void init_queue_managers();
        void init_thread_resources();

        void init_default_pipeline();

        //TODO: use dynamic uniform_buffer
        void init_static_uniform_buffers();
        void init_dynamic_uniform_buffers();

        void init_descriptors();

        void update_uniform_buffers();

        nest::command_commander* prepare_render_targets();
    public:
        renderer();

        void copy_render_target(model& model);
        void add_render_target(sz::string_view path_to_model);

        renderer* bind_custom_thread_resource(nest::resources::command_thread_resource&& resource);
        renderer* bind_custom_swapchain(nest::wrappers::swapchain&& chain);
        renderer* bind_custom_graphics_pipeline(nest::pipeline::graphics_pipeline&& pipeline);

        renderer* change_viewport_sizes(float x, float y, float width, float height);

        void add_graphics_pipeline(std::unique_ptr<nest::pipeline::graphics_pipeline_ci> ci, sz::string_view pipeline_id);
        void add_graphics_pipeline(std::unique_ptr<nest::shaders::shader_set>& set, sz::string_view father_pipeline, sz::string_view pipeline_id = ":derivative");
        void add_swapchain(nest::wrappers::swapchain_ci&& ci);
        void add_queue_manager(nest::wrappers::queue_family family);
        void add_thread_resource(nest::wrappers::queue_type type);

        void fast_load_texture_into_model(model* md);

        std::unique_ptr<model>& get_render_target(sz::string_view model_id);


        renderer* set_cam(camera cam);
        renderer* render();

        void show_on_screen();
    };
}
#endif //HARPY_3D_RENDERER