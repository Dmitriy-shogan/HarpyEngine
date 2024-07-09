#include <nest/texturing/texture.h>


void harpy::nest::texturing::texture::init()
{
    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;
    
    ci.extent.width = cv_image->get_cv_data().cols;
    ci.extent.height = cv_image->get_cv_data().rows;
    ci.extent.depth = 1;

    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    //Just for now
    ci.mipLevels = 1;
    ci.samples = VK_SAMPLE_COUNT_1_BIT;
    ci.arrayLayers = 1;
    
    switch(cv_image->get_cv_data().channels())
    {
    case 1:
        ci.format = VK_FORMAT_R8_SRGB;
        break;
    case 3:
        ci.format = VK_FORMAT_B8G8R8_SRGB;
        break;
    case 4:
        ci.format = VK_FORMAT_B8G8R8A8_SRGB;
        break;
    default:
        throw utilities::error_handling::harpy_little_error("WATAFUCK, you have something strange number of channels, while initialising texture");
    }
    
    //Just for now
    ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_ci{};
    alloc_ci.usage = VMA_MEMORY_USAGE_AUTO;

    HARPY_VK_CHECK(vmaCreateImage(*allocator, &ci, &alloc_ci, &this->image, &allocation, nullptr));
    init_view(ci.format);
}

void harpy::nest::texturing::texture::init_view(VkFormat format)
{
    VkImageViewCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ci.format = format;
    ci.image = image;
    ci.viewType = VK_IMAGE_VIEW_TYPE_2D;

    //Just for now
    ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ci.subresourceRange.baseMipLevel = 0;
    ci.subresourceRange.levelCount = 1;
    ci.subresourceRange.baseArrayLayer = 0;
    ci.subresourceRange.layerCount = 1;

    HARPY_VK_CHECK(vkCreateImageView(*device, &ci, nullptr, &view));
}

harpy::nest::texturing::texture::texture(utilities::images::image& image, VkDevice* device,
                                         VmaAllocator* allocator)
        : device(device),
          allocator(allocator),
cv_image(&image)
{
    init();
}

harpy::nest::texturing::texture::texture(texture&& text) noexcept : device(text.device),
                                                                    allocator(text.allocator),
                                                                    image(text.image),
                                                                    allocation(text.allocation),
cv_image(text.cv_image)
{
    text.image = nullptr;
    text.allocation = nullptr;
}

harpy::nest::texturing::texture& harpy::nest::texturing::texture::operator=(texture&& text) noexcept
{
    device = text.device;
    allocator = text.allocator;
    image = text.image;
    allocation = text.allocation;
    cv_image = text.cv_image;

    
    text.allocation = nullptr;
    text.image = nullptr;
    return *this;
}

VkImage& harpy::nest::texturing::texture::get_vk_image()
{return image;
}

harpy::nest::texturing::texture::operator VkImage_T*&()
{return image;
}

harpy::utilities::images::image* harpy::nest::texturing::texture::get_raw_cv_image_ptr()
{return cv_image;
}

size_t harpy::nest::texturing::texture::get_size()
{
    return cv_image->get_cv_data().total() * cv_image->get_cv_data().elemSize();
}


harpy::nest::texturing::texture::~texture()
{
    if(image)
    {
        vkDestroyImageView(*device, view, nullptr);
        vmaDestroyImage(*allocator, image, allocation);
    }
}
