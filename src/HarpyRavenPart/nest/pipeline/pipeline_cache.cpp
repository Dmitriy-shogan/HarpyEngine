#include <nest/pipeline/pipeline_cache.h>
#include <nest/resources/common_vulkan_resource.h>
#include <nest/pipeline/graphics_pipeline.h>
#include <filesystem>
#include <logger/harpy_little_error.h>
#include <logger/logger.h>

using harpy_error = harpy::utilities::harpy_little_error; 
using logger = harpy::utilities::logger;
using resource = harpy::nest::resources::common_vulkan_resource;

#define HARPY_STD_PIPELINE_CACHE_FILE "data/pipeline_cache.hpc"

harpy::nest::pipeline::pipeline_cache::pipeline_cache(bool do_restore,  VkDevice* device) : device(device)
{
    if(do_restore)
    {
        restore_cache();
    } else {
        VkPipelineCacheCreateInfo ci{};
        ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        vkCreatePipelineCache(*device, &ci, nullptr, &cache);
    }
}


void harpy::nest::pipeline::pipeline_cache::save_to_file(sz::string_view directory, sz::string_view filename)
{
    if(filename.empty())
        filename =  "pipeline_cache.hpc"; //Harpy Pipeline Cache
    sz::string full_name {sz::string{directory}.append("\\").append(filename)};

    std::fstream out{full_name, std::ios_base::app};
    if(!out.is_open())
    {
        std::filesystem::create_directory(directory.begin());
        std::ofstream{full_name};
        out.open(full_name);
    }

    size_t cache_size{};
    vkGetPipelineCacheData(*device, cache, &cache_size, nullptr);
    sz::string local_cache{};
    local_cache.resize(cache_size);
    vkGetPipelineCacheData(*device, cache, &cache_size, local_cache.data());
    out << local_cache << std::endl;

    out.close();
}

void harpy::nest::pipeline::pipeline_cache::restore_cache(sz::string_view directory, sz::string_view filename)
{
    if(cache)
        vkDestroyPipelineCache(*device, cache, nullptr);

    if(filename.empty())
        filename = "pipeline_cache.hpc";

    sz::string full_name {sz::string{directory}.append("\\").append(filename)};

    std::ifstream in{full_name};
    if (!in.is_open())
    {
        throw harpy_error("You haven't created any caches yet to restore them or it hasn't been found");
    }
    std::string cache{};
    std::getline(in, cache);

    VkPipelineCacheCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    ci.initialDataSize = cache.size();
    ci.pInitialData = cache.data();

    vkCreatePipelineCache(*device, &ci, nullptr, &this->cache);
}

VkPipelineCache& harpy::nest::pipeline::pipeline_cache::get_vk_cache()
{ return cache;
}

harpy::nest::pipeline::pipeline_cache::operator VkPipelineCache_T*&()
{ return cache;
}

harpy::nest::pipeline::pipeline_cache::~pipeline_cache()
{
    vkDestroyPipelineCache(*device, cache, nullptr);
}
