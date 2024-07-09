#include <nest/texturing/texture_sampler.h>

harpy::nest::texturing::texture_sampler::texture_sampler(texture_sampler&& other) noexcept: device{other.device},
    sampler{other.sampler}
{
    other.sampler = nullptr;
}

harpy::nest::texturing::texture_sampler& harpy::nest::texturing::texture_sampler::operator=(
    texture_sampler&& other) noexcept
{
    if (this == &other)
        return *this;
    device = other.device;
    sampler = other.sampler;
    other.sampler = nullptr;
    return *this;
}

VkSampler& harpy::nest::texturing::texture_sampler::get_vk_sampler()
{
    return sampler;
}

harpy::nest::texturing::texture_sampler::operator VkSampler_T*&()
{
    return sampler;
}

harpy::nest::texturing::texture_sampler::texture_sampler(texture_address_modes addresses, VkDevice* device)
{
    VkSamplerCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    ci.magFilter = VK_FILTER_LINEAR;
    ci.minFilter = VK_FILTER_NEAREST;

    ci.addressModeU = addresses.U;
    ci.addressModeV = addresses.V;
    ci.addressModeW = addresses.W;

    auto& phys_props = resources::common_vulkan_resource::get_resource().get_phys_props();

    ci.anisotropyEnable = true;
    //Just for now, -1 for better performance
    ci.maxAnisotropy = phys_props.properties.limits.maxSamplerAnisotropy - 1;

    ci.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    ci.unnormalizedCoordinates = false;

    //Just for now
    ci.compareEnable = VK_FALSE;
    ci.compareOp = VK_COMPARE_OP_ALWAYS;

    //Just for now
    ci.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    ci.mipLodBias = 0.0f;
    ci.minLod = 0.0f;
    ci.maxLod = 0.0f;

    HARPY_VK_CHECK(vkCreateSampler(*device, &ci, nullptr, &sampler));
}

harpy::nest::texturing::texture_sampler::~texture_sampler()
{
    if(sampler)
        vkDestroySampler(*device, sampler, nullptr);
}
