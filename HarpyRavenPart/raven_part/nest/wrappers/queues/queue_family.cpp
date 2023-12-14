#include "queue_family.h"
#include <nest/resources/common_vulkan_resource.h>

using resource = harpy::nest::resources::common_vulkan_resource;

VkQueue& harpy::nest::wrappers::queue_family::get_vk_queue(std::size_t index)
{
    return queues[index];
}

void harpy::nest::wrappers::queue_family::init()
{
    for(unsigned i = 0; i < queue_amount; i++)
    {
        queues.push_back({});
        vkGetDeviceQueue(resources::common_vulkan_resource::get_resource(), family_number, i, &queues.back());
        VkBool32 do_support;
        vkGetPhysicalDeviceSurfaceSupportKHR(resources::common_vulkan_resource::get_resource(), i, resources::common_vulkan_resource::get_resource(), &do_support);
        do_support_present = do_support;
    }
}

harpy::nest::wrappers::queue_family& harpy::nest::wrappers::queue_family::operator=(queue_family const& family)
{
    if(resource::get_resource().is_inited())
        init();
    return *this;
}

harpy::nest::wrappers::queue_family::queue_family(queue_family const & family) : queue_amount(family.queue_amount), type(family.type)
{
    if(resource::get_resource().is_inited())
        init();
}

std::vector<VkQueue>& harpy::nest::wrappers::queue_family::get_all_vk_queues()
{
    return queues;
}

harpy::nest::wrappers::queue_family::operator VkQueue&()
{
    return queues[0];
}

harpy::nest::wrappers::queue_type harpy::nest::wrappers::queue_family::get_type() const
{
    return type;
}


uint32_t harpy::nest::wrappers::queue_family::get_family_index()
{return family_number;}

harpy::nest::wrappers::queue_family::operator unsigned()
{return family_number;}

bool harpy::nest::wrappers::queue_family::do_support_presentation()
{
    return do_support_present;
}

unsigned harpy::nest::wrappers::queue_family::size()
{return queue_amount;
}

harpy::nest::wrappers::queue_family::~queue_family() = default;
