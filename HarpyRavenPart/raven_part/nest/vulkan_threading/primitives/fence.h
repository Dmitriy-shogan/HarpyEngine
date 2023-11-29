#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVE_FENCES
#define HARPY_NEST_THREADING_PRIMITIVE_FENCES
#include <volk/volk.h>

namespace harpy::nest::threading::primitives
{
    //CPU sync
    class fence
    {
        VkDevice* device{};
        VkFence vk_fence{};
        
    public:
        void init(bool is_signaled, VkDevice* device);
        VkResult get_status();

        VkFence& get_vk_fence();
        operator VkFence&();
        ~fence();
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVE_FENCES