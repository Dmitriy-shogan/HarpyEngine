#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVE_EVENT
#define HARPY_NEST_THREADING_PRIMITIVE_EVENT
#include <volk/volk.h>

namespace harpy::nest::threading::primitive
{
    class event
    {
        VkDevice* device{};
        VkEvent vk_event{};
        
    public:
        
        void init(VkDevice* device);

        operator VkEvent&();
        VkEvent& get_event();

        ~event();
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVE_EVENT