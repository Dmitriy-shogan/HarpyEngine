#include <nest/wrappers/queues/queue_family.h>
#include <nest/resources/common_vulkan_resource.h>

using resource = harpy::nest::resources::common_vulkan_resource;

harpy::nest::wrappers::queue_family::queue_family(queue_type q_type, uint32_t family, unsigned queue_amount): family_number(family)
    , type(q_type)
    , queue_amount(queue_amount)
{}

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
    VkBool32 do_support;
    vkGetPhysicalDeviceSurfaceSupportKHR(resources::common_vulkan_resource::get_resource(), family_number, resources::common_vulkan_resource::get_resource(), &do_support);
    return do_support;
}

unsigned harpy::nest::wrappers::queue_family::size()
{return queue_amount;
}

harpy::nest::wrappers::queue_family::~queue_family() = default;
