#pragma once
#ifndef HARPY_BASE_TEXTURE
#define HARPY_BASE_TEXTURE
#include <utilities/harpy_little_error.h>
#include <buffers/staging_buffer.h>
#include <image_processor/image_processor.h>

namespace harpy::nest::texturing
{
    //Remember, opecv2 uses brg instead of rgb
    
    class base_texture
    {
        VkImage texture{};
        VkDeviceMemory texture_memory{};
        queues::transfer_queue* queue;
        //TEMPORAL vulkan_backend HERE, later change to queue and devices
        vulkan_spinal_cord& vulkan_backend;
        //Thinking about going static with pool, need more research
        pools::command_pool pool{vulkan_backend.get_vk_device(), pools::command_pool_types::copy};
        
    public:
        base_texture(vulkan_spinal_cord& vulkan_backend, queues::transfer_queue* queue) : vulkan_backend(vulkan_backend),
        queue(queue){}

        void init(std::string_view filepath)
        {
            cv::Mat image;
            std::vector<unsigned char> raw;
            try
            {
                image = image_processor::read_image(filepath.data());
                raw = image_processor::get_raw_pixels(image);
            }
            catch (std::runtime_error& error)
            {
                throw utilities::harpy_little_error(utilities::error_severity::cant_find_file, error.what());
            }
            VkDeviceSize image_size = raw.size() * sizeof(raw[0]);
            buffers::staging_buffer stage{pool, vulkan_backend, queue};
            stage.init(image_size);
            

            VkImageCreateInfo image_info{};
            image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            image_info.imageType = VK_IMAGE_TYPE_2D;
            image_info.extent.width = static_cast<uint32_t>(image.cols * image.channels());
            image_info.extent.height = static_cast<uint32_t>(image.rows);
            image_info.extent.depth = 1;
            image_info.mipLevels = 1;
            image_info.arrayLayers = 1;
            image_info.format = VK_FORMAT_B8G8R8_SRGB;
            image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
            image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            image_info.samples = VK_SAMPLE_COUNT_1_BIT;
            image_info.flags = 0; // Optional

            if (vkCreateImage(vulkan_backend.get_vk_device(), &image_info, nullptr, &texture) != VK_SUCCESS) {
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to create image!");
            }
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(vulkan_backend.get_vk_device(), texture, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = utilities::find_memory_types(vulkan_backend.get_vk_physical_device(), memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            if (vkAllocateMemory(vulkan_backend.get_vk_device(), &allocInfo, nullptr, &texture_memory) != VK_SUCCESS) {
                throw utilities::harpy_little_error(utilities::error_severity::wrong_init,"failed to allocate image memory!");
            }
            vkBindImageMemory(vulkan_backend.get_vk_device(), texture, texture_memory, 0);
        }

        //TEMPORAL FUNCTION FOR TUTORIAL, REMOVE LATER
        static void createImage(VkDevice& device, VkPhysicalDevice& ph_device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = width;
            imageInfo.extent.height = height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = format;
            imageInfo.tiling = tiling;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = usage;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image!");
            }

            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(device, image, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = utilities::find_memory_types(ph_device, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


            if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate image memory!");
            }

            vkBindImageMemory(device, image, imageMemory, 0);
        }
    
    };
}


#endif //HARPY_BASE_TEXTURE