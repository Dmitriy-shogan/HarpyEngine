#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVE_EVENT
#define HARPY_NEST_THREADING_PRIMITIVE_EVENT
#include "nest/resources/common_vulkan_resource.h"

namespace harpy::nest::threading
{
    //Both CPU and GPU
    class event
    {
        VkDevice* device{};
        VkEvent vk_event{};
        
    public:

        event(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        void set();
        void reset();
        VkResult get_status();

        operator VkEvent&();
        VkEvent& get_event();

        ~event();
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVE_EVENT