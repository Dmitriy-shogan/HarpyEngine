#pragma once
#ifndef HARPY_NEST_WRAPPERS_PIPELINE
#define HARPY_NEST_WRAPPERS_PIPELINE
#include <volk/volk.h>
#include <vector>
#include <nest/shader_works/shader_module/shader_module.h>

#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::wrappers
{
    class render_pass;
    class pipeline_cache;
    static const std::vector<VkDynamicState> standard_dynamic_states{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    struct graphics_pipeline_options_ci;
    struct graphics_pipeline_ci;
    

    //TODO: create graphics pipeline families
    class graphics_pipeline
    {
        VkPipeline pipe{};
        VkPipelineLayout layout{};
        std::string id{};

        void init_layout();

    public:

        graphics_pipeline() = default;

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