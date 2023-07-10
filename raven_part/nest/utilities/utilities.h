#pragma once
#ifndef HARPY_UTILS
#define HARPY_UTILS
#include <utilities/initializations.h>


#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <optional>
#include <string>
#include <set>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <exception>
//#include <format>
#include <functional>
#include <array>
//libfmt
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <string.h>
#include <memory>

using namespace harpy::nest;
namespace harpy::utilities {
    
    std::vector<char> read_file(std::string filepath);

    std::ostream& operator <<(std::ostream& out, nest::vertex& vertex);

    //VERY rude way to copy buffers
    //TODO: make vk_copy_buffer more friendly, make warnings for transfer queue not being transfer
    //void vk_copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size, VkDevice& device, std::shared_ptr<pools::command_pool>  pool, VkQueue transfer_queue);

    uint32_t find_memory_types (VkPhysicalDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
};


#endif //HARPY_UTILS
