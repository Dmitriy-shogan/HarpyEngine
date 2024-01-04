#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include <utilities/initializations.h>
#include <utilities/harpy_little_error.h>
#include <utilities/buffers/buffer.h>
#include <utilities/gltf.h>
#include <utilities/property.h>



using namespace harpy::nest;
namespace harpy::utilities {
    
    std::vector<char> read_file(std::string filepath);

    uint32_t find_memory_types (VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkDeviceSize align_to(VkDeviceSize val, uint32_t bound);
    uint32_t getChannelCountForFormat(VkFormat format);
    uint32_t getVulkanComponentSize(VkFormat format);
    bool isFloat(VkFormat format);
    bool isUInt(VkFormat format);
    bool isSInt(VkFormat format);

};


#endif //HARPY_UTILS
