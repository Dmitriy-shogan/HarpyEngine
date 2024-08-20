#pragma once
#ifndef HARPY_3D_RENDERER
#define HARPY_3D_RENDERER
#include <nest/command_commander/command_commander.h>

namespace harpy::D3
{
    //TODO: rethink renderer
    class renderer
    {
        uint32_t image_index{0};

        nest::command_commander commander{};

        //Just for now. Will be needed more swapchains to support multiple screens.
        nest::wrappers::swapchain swapchain;

        std::vector<nest::resources::command_thread_resource> thread_resources{};

        std::vector<nest::pipeline::graphics_pipeline> pipelines{};
        std::vector<nest::pipeline::pipeline_cache> pipeline_caches{};

        std::vector<nest::threading::semaphore> semaphores{};
        std::vector<nest::threading::fence> fences{};

        nest::threading::semaphore render_finish_sem{};

        
    public:
        renderer(nest::wrappers::swapchain swapchain);

        
        void show_on_screen();
    };
}
#endif //HARPY_3D_RENDERER