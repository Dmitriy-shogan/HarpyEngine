#pragma once
#ifndef HARPY_NEST_WRAPPERS_PIPELINE_CACHE
#define HARPY_NEST_WRAPPERS_PIPELINE_CACHE
#include <string>
#include <volk/volk.h>

namespace harpy::nest::wrappers
{
    class graphics_pipeline;
    class pipeline_cache
    {
        std::string id{};
        VkPipelineCache cache{};
    
    public:

        pipeline_cache(std::string id = {"first"});

        void init(bool do_restore = false);

        void set_id(std::string id);

        void save_to_file();
        std::string restore_cache();

        VkPipelineCache& get_vk_cache();
        operator VkPipelineCache&();

        ~pipeline_cache();
    };
}

#endif//HARPY_NEST_WRAPPERS_PIPELINE_CACHE