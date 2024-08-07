#pragma once
#ifndef HARPY_NEST_TEXTURING_SAMPLER
#define HARPY_NEST_TEXTURING_SAMPLER
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::texturing
{
    struct texture_address_modes
    {
        VkSamplerAddressMode
          U{VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER}
        , V{VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER}
        , W{VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER};
    };

    class texture_sampler
    {
    private:
        VkDevice* device{};
        VkSampler sampler{};
        
    public:
        texture_sampler(texture_address_modes addresses = {},
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        texture_sampler(const texture_sampler& other) = delete;
        texture_sampler(texture_sampler&& other) noexcept;

        texture_sampler& operator=(const texture_sampler& other) = delete;
        texture_sampler& operator=(texture_sampler&& other) noexcept;

        VkSampler& get_vk_sampler();
        operator VkSampler&();


        
        ~texture_sampler();
    };
}


#endif //HARPY_NEST_TEXTURING_SAMPLER