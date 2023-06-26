#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include <utilities/initializations.h>

namespace harpy::utilities {
    
    std::vector<char> read_file(std::string filepath);

    std::ostream& operator <<(std::ostream& out, nest::vertex& vertex);
    
    uint32_t find_memory_types (VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    nest::needed_queues_families find_queue_families(VkPhysicalDevice& ph_device, VkSurfaceKHR& surface);
};


#endif //HARPY_UTILS
