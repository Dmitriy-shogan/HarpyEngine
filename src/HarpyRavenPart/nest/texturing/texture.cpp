#include <nest/texturing/texture.h>


void harpy::nest::texturing::texture::init(utilities::image& image)
{
    image_size = image.get_cv_data().total() * image.get_cv_data().elemSize();

    VkImageCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ci.imageType = VK_IMAGE_TYPE_2D;
    
    ci.extent.width = image.get_cv_data().cols;
    ci.extent.height = image.get_cv_data().rows;
    ci.extent.depth = 1;
    this->extent = ci.extent;

    ci.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    //Just for now
    ci.mipLevels = 1;
    ci.samples = VK_SAMPLE_COUNT_1_BIT;
    ci.arrayLayers = 1;
    bool checker{false};
    switch(image.get_cv_data().channels())
    {
    case 1: {
        ci.format = VK_FORMAT_R8_SRGB;
        break;
    }
    case 3: {
        image.convert_image_color_space(cv::COLOR_BGR2BGRA);
    }
    case 4: {
        ci.format = VK_FORMAT_B8G8R8A8_SRGB;
        break;
    }
    default:
        throw utilities::harpy_little_error("WATAFUCK, you have something strange number of channels, while initialising texture");
    }
    
    //Just for now
    ci.tiling = VK_IMAGE_TILING_OPTIMAL;
    ci.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(*device, &ci, nullptr, &this->image);

    VmaAllocationCreateInfo alloc_ci{};
    alloc_ci.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    vmaCreateImage(*allocator, &ci, &alloc_ci, &this->image, &allocation, nullptr);
    init_view(ci.format);

    if(checker) {
        /*image.convert_image_color_space(cv::COLOR_RGB2BGR);*/}
}

void harpy::nest::texturing::texture::init_view(VkFormat format)
{
    this->format = format;
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

harpy::nest::texturing::texture::texture(utilities::image& image, VkDevice* device,
                                         VmaAllocator* allocator)
        : device(device),
          allocator(allocator)
{
    init(image);
}

harpy::nest::texturing::texture::texture(VkDevice* device,
                                         VmaAllocator* allocator)
        : device(device),
          allocator(allocator)
{
}

harpy::nest::texturing::texture::texture(texture&& text) noexcept : device(text.device),
                                                                    allocator(text.allocator),
                                                                    image(text.image),
                                                                    allocation(text.allocation)
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

size_t harpy::nest::texturing::texture::get_size()
{
    return image_size;
}


harpy::nest::texturing::texture::~texture()
{
    if(image)
    {
        vkDestroyImageView(*device, view, nullptr);
        vmaDestroyImage(*allocator, image, allocation);
    }
}

VkImageView &harpy::nest::texturing::texture::get_vk_image_view() {
    return view;
}

harpy::nest::texturing::texture::operator VkImageView &() {
    return view;
}
