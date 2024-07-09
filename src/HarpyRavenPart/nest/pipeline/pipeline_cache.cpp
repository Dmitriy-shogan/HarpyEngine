#include <nest/pipeline/pipeline_cache.h>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/pipeline/graphics_pipeline.h>
#include <filesystem>
#include <logger/harpy_little_error.h>
#include <logger/logger.h>

using harpy_error = harpy::utilities::error_handling::harpy_little_error; 
using logger = harpy::utilities::error_handling::logger;
using resource = harpy::nest::resources::common_vulkan_resource;

#define HARPY_STD_PIPELINE_CACHE_FILE "data/pipeline_cache.hpc"

harpy::nest::pipeline::pipeline_cache::pipeline_cache(std::string id, bool do_restore) : id(std::move(id))
{
    VkPipelineCacheCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    if(do_restore)
    {
        try{
            auto data = restore_cache();
            ci.initialDataSize = data.size();
            ci.pInitialData = data.c_str();
        }
        catch(harpy_error& err)
        {
            logger::get_logger().log(err);
            if constexpr(is_harpy_debug)
                logger::get_logger().show_last_log();
        }
    }

    vkCreatePipelineCache(resource::get_resource(), &ci, nullptr, &cache);
}

void harpy::nest::pipeline::pipeline_cache::set_id(std::string id)
{
    this->id = std::move(id);
}

void harpy::nest::pipeline::pipeline_cache::save_to_file()
{
    std::ofstream out{HARPY_STD_PIPELINE_CACHE_FILE, std::ios_base::app}; //Harpy Pipeline Cache
    if(!out.is_open())
    {
        std::filesystem::create_directory("data");
        std::ofstream{HARPY_STD_PIPELINE_CACHE_FILE};
        out.open(HARPY_STD_PIPELINE_CACHE_FILE);
    }

    out << std::endl << id << ':';

    size_t cache_size{};
    vkGetPipelineCacheData(resource::get_resource(), cache, &cache_size, nullptr);
    std::string local_cache{};
    local_cache.resize(cache_size);
    vkGetPipelineCacheData(resource::get_resource(), cache, &cache_size, local_cache.data());
    out << local_cache;

    out.close();
}

std::string harpy::nest::pipeline::pipeline_cache::restore_cache()
{
    std::ifstream in{HARPY_STD_PIPELINE_CACHE_FILE};
    if (!in.is_open())
    {
        throw harpy_error("You haven't created any caches yet to restore them.");
    }
    std::string local_cache{};
    while (std::getline(in, local_cache))
    {
        if(local_cache.find(id) != std::string::npos)
        {
            auto index = local_cache.find_first_of(':');
            return std::string{local_cache.substr(++index)};
        }
    }
    throw harpy_error("There is no pipelines with this id");
}

VkPipelineCache& harpy::nest::pipeline::pipeline_cache::get_vk_cache()
{ return cache;
}

harpy::nest::pipeline::pipeline_cache::operator VkPipelineCache_T*&()
{ return cache;
}

harpy::nest::pipeline::pipeline_cache::~pipeline_cache()
{
    vkDestroyPipelineCache(resource::get_resource(), cache, nullptr);
}
