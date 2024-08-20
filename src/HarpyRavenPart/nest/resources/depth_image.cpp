//
// Created by Xyeveloper on 25.07.2024.
//

#include <nest/resources/depth_image.h>
#include "nest/wrappers/swapchain.h"

void harpy::nest::resources::depth_image::init(VkExtent2D extent)
{

    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;

    ci.extent = {extent.width, extent.height, 1};
    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    //Just for now
    ci.mipLevels = 1;
    ci.samples = VK_SAMPLE_COUNT_1_BIT;
    ci.arrayLayers = 1;
    ci.format = wrappers::swapchain::get_depth_format();

    //Just for now
    ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    ci.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_ci{};
    alloc_ci.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    alloc_ci.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    HARPY_VK_CHECK(vmaCreateImage(*allocator, &ci, &alloc_ci, &this->image, &allocation, nullptr));
    init_view();
}

void harpy::nest::resources::depth_image::init_view()
{

    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.format = wrappers::swapchain::get_depth_format();
    ci.image = image;
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;

    //Just for now
    ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;

    HARPY_VK_CHECK(vkCreateImageView(*device, &ci, nullptr, &view));
}

harpy::nest::resources::depth_image::depth_image(VkExtent2D extent, VkDevice* device,
                                         VmaAllocator* allocator)
        : device(device),
          allocator(allocator) {
    if (extent.width || extent.height)
        init(extent);
}

harpy::nest::resources::depth_image::depth_image(depth_image&& depth) noexcept : device(depth.device),
                                                                    allocator(depth.allocator),
                                                                    image(depth.image),
                                                                    allocation(depth.allocation)
{
    depth.image = nullptr;
    depth.allocation = nullptr;
}

harpy::nest::resources::depth_image& harpy::nest::resources::depth_image::operator=(depth_image&& depth) noexcept
{
    device = depth.device;
    allocator = depth.allocator;
    image = depth.image;
    allocation = depth.allocation;


    depth.allocation = nullptr;
    depth.image = nullptr;
    return *this;
}

VkImage& harpy::nest::resources::depth_image::get_vk_image()
{return image;
}

harpy::nest::resources::depth_image::operator VkImage_T*&()
{return image;
}

harpy::nest::resources::depth_image::~depth_image()
{
    if(image)
    {
        vkDestroyImageView(*device, view, nullptr);
        vmaDestroyImage(*allocator, image, allocation);
    }
}

VkImageView &harpy::nest::resources::depth_image::get_vk_image_view() {
    return view;
}

harpy::nest::resources::depth_image::operator VkImageView &() {
    return view;
}
