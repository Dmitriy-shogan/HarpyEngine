#include <nest/vulkan_threading/event.h>

harpy::nest::threading::event::event(VkDevice* device) : device (device)
{
    
    VkEventCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
    
    HARPY_VK_CHECK(vkCreateEvent(*device, &ci, nullptr, &vk_event));
}

void harpy::nest::threading::event::set()
{
    vkSetEvent(*device, vk_event);
}

void harpy::nest::threading::event::reset()
{
    vkResetEvent(*device, vk_event);
}

VkResult harpy::nest::threading::event::get_status()
{
    return vkGetEventStatus(*device, vk_event);
}

harpy::nest::threading::event::operator VkEvent_T*&()
{return vk_event;
}

VkEvent& harpy::nest::threading::event::get_event()
{return vk_event;
}


harpy::nest::threading::event::~event()
{
    if(vk_event)
    {
        vkDestroyEvent(*device, vk_event, nullptr);
    }
}
