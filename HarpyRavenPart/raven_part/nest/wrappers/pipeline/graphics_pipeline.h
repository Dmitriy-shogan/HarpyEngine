#pragma once
#ifndef HARPY_NEST_WRAPPERS_PIPELINE
#define HARPY_NEST_WRAPPERS_PIPELINE
#include <nest/shader_works/shader_module/shader_module.h>
#include <nest/wrappers/render_pass/render_pass.h>
#include <nest/wrappers/pipeline/pipeline_cache.h>

#include "nest/resources/common_vulkan_resource.h"

namespace harpy::nest::wrappers
{
    static const std::vector standard_dynamic_states{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    struct graphics_pipeline_options_ci;
    struct graphics_pipeline_ci;
    
    class graphics_pipeline
    {
        VkPipeline pipe{};
        VkPipelineLayout layout{};
        std::string id{};

        void init_layout();

    public:

        /*graphics_pipeline(graphics_pipeline& buffer) noexcept;
        graphics_pipeline(const graphics_pipeline& buffer) noexcept;
        graphics_pipeline& operator=(graphics_pipeline& buffer) noexcept;
        graphics_pipeline& operator=(const graphics_pipeline& buffer) noexcept;

        graphics_pipeline(graphics_pipeline&& buffer) noexcept;
        graphics_pipeline& operator= (graphics_pipeline&& buffer) noexcept;*/

        void init(std::shared_ptr<graphics_pipeline_ci> create_info,
            bool is_wireframe = false,
            VkDevice device = resources::common_vulkan_resource::get_resource().get_main_device());

        VkPipeline& get_vk_pipeline();
        operator VkPipeline&();

        VkPipelineLayout& get_vk_layout();
        operator VkPipelineLayout&();

        void set_id(std::string id);
        std::string get_id();

        
    ~graphics_pipeline();
    };
}
#endif //HARPY_NEST_WRAPPERS_PIPELINE