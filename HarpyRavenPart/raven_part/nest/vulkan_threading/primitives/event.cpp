#include "event.h"
#include <nest/initializations/inititalizations.h>

#include "nest/resources/common_vulkan_resource.h"

void harpy::nest::threading::primitive::event::init(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device())
{
    this->device = device;




    
    HARPY_VK_CHECK(vkCreateEvent(*device, &ci, nullptr, &vk_event));
}

harpy::nest::threading::primitive::event::operator VkEvent_T*&()
{return vk_event;
}

VkEvent& harpy::nest::threading::primitive::event::get_event()
{return vk_event;
}

harpy::nest::threading::primitive::event::~event()
{
    if(vk_event)
    {
        vkDestroyEvent(*device, vk_event, nullptr);
    }
}
