#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVE_FENCES
#define HARPY_NEST_THREADING_PRIMITIVE_FENCES
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::threading
{
    //CPU sync
    class fence
    {
        bool do_delete_in_destructor = true;
        VkDevice* device{};
        VkFence vk_fence{};
        
    public:

        fence(bool is_signaled = false, VkDevice* device  = &resources::common_vulkan_resource::get_resource().get_main_device());
        fence(fence& fen);
        fence(fence&& fen);
        
        VkResult get_status();

        VkFence& get_vk_fence();
        operator VkFence&();

        [[maybe_unused]] VkResult wait_for_fence(std::size_t how_long_to_wait = LLONG_MAX);
        [[maybe_unused]] VkResult wait_and_reset_fence(std::size_t how_long_to_wait = LLONG_MAX);
        static VkResult wait_for_fences(std::vector<fence> fences,
        std::size_t how_long_to_wait = LLONG_MAX,
            VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        ~fence();
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVE_FENCES