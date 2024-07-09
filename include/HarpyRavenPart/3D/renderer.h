#pragma once
#ifndef HARPY_NEST_3D_RENDERER
#define HARPY_NEST_3D_RENDERER
#include <nest/command_commander/command_commander.h>

namespace harpy::nest::D3
{
    class renderer
    {
        command_commander commander{};
        //Just for now, need multiple swapchain support to render to not only main window
        std::unique_ptr<wrappers::swapchain> swapchain{};

        std::vector<resources::command_thread_resource> thread_resources{};

        std::vector<pipeline::graphics_pipeline> pipelines{};
        std::vector<pipeline::pipeline_cache> pipeline_caches{};
        std::vector<shaders::shader_set> shader_sets{};

        std::vector<threading::semaphore> semaphores{};
        std::vector<threading::fence> fences{};
        
        const resources::common_vulkan_resource& resource = resources::common_vulkan_resource::get_resource();

        uint32_t image_index{};
        
    public:

        
        void show_on_screen();
    };
}
#endif //HARPY_NEST_3D_RENDERER