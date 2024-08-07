//
// Created by Xyeveloper on 25.07.2024.
//
#pragma once
#ifndef HARPY_NEST_RESOURCES_DEPTH_IMAGE
#define HARPY_NEST_RESOURCES_DEPTH_IMAGE
#include <nest/resources/common_vulkan_resource.h>
#include "nest/wrappers/swapchain/swapchain.h"

namespace harpy::nest::resources {

    class depth_image {
        VkDevice* device{};
        VmaAllocator* allocator{};

        VkImage image{};
        VmaAllocation allocation{};
        VkImageView view{};

        void init(wrappers::swapchain& chain);
        void init_view();
    public:

        depth_image(wrappers::swapchain& chain,
                VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device(),
                VmaAllocator* allocator = &resources::common_vulkan_resource::get_resource().get_main_allocator());

        depth_image(const depth_image& depth) = delete;
        depth_image(depth_image&& depth) noexcept;

        depth_image& operator=(const depth_image& depth) = delete;
        depth_image& operator=(depth_image&& depth) noexcept;

        VkImage& get_vk_image();
        operator VkImage&();

        VkImageView& get_vk_image_view();
        operator VkImageView&();

        ~depth_image();
    };

}

#endif //HARPY_NEST_RESOURCES_DEPTH_IMAGE
