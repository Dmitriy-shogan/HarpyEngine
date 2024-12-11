#pragma once
#ifndef HARPY_NEST_PIPELINE_PIPELINE_CACHE
#define HARPY_NEST_PIPELINE_PIPELINE_CACHE
#include <string>
#include <volk.h>
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::pipeline
{
    class graphics_pipeline;
    class pipeline_cache
    {
        VkPipelineCache cache{};
        VkDevice* device{};
    
    public:

        pipeline_cache(bool do_restore = false, VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());


        void save_to_file(sz::string_view directory = "data/caches", sz::string_view filename = "");
        void restore_cache(sz::string_view directory = "data/caches", sz::string_view filename = "");

        VkPipelineCache& get_vk_cache();
        operator VkPipelineCache&();

        ~pipeline_cache();
    };
}

#endif//HARPY_NEST_PIPELINE_PIPELINE_CACHE