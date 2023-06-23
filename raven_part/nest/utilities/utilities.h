#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include <utilities/initializations.h>

namespace harpy::utilities {
    
    std::vector<char> read_file(std::string filepath);

    std::ostream& operator <<(std::ostream& out, nest::vertex& vertex);

    //VERY rude way to copy buffers
    //TODO: make vk_copy_buffer more friendly, make warnings for transfer queue not being transfer
    void vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDevice& device, VkCommandPool& pool, VkQueue transfer_queue);

    uint32_t find_memory_types (VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};


#endif //HARPY_UTILS
