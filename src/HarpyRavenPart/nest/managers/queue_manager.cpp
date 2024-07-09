#include <nest/managers/queue_manager.h>

#include <nest/resources/common_vulkan_resource.h>

harpy::nest::managers::queue_wrapper::queue_wrapper(VkQueue queue, wrappers::queue_type type)
: type(type), queue(queue)
{}

VkQueue& harpy::nest::managers::queue_wrapper::get_queue()
{return queue;
}

harpy::nest::wrappers::queue_type harpy::nest::managers::queue_wrapper::get_type()
{return type;
}

harpy::nest::managers::queue_wrapper::operator  harpy::nest::wrappers::queue_type  const&() const 
{return type;}

harpy::nest::managers::queue_wrapper::operator VkQueue_T* const&() const
{return queue;}

harpy::nest::managers::queue_wrapper::operator VkQueue_T* const*() const
{return &queue;}

harpy::nest::managers::queue_wrapper::~queue_wrapper()
{manager->queues.push_back(queue);}



harpy::nest::managers::queue_manager::queue_manager(wrappers::queue_family& family)
{
    family_index = family.get_family_index();
    type = family.get_type();
    queues.resize(family.size());
    
    for(unsigned i = 0; i < family.size(); i++)
    {
        vkGetDeviceQueue(resources::common_vulkan_resource::get_resource(), family.get_family_index(), i, &queues[i]);
    }
}

harpy::nest::managers::queue_wrapper harpy::nest::managers::queue_manager::get_queue()
{
    queue_wrapper wrapper{queues.front(), type};
    wrapper.manager = this;
    queues.pop_front();
    return wrapper;
}

std::deque<VkQueue>& harpy::nest::managers::queue_manager::get_all_available_queues()
{
    return queues;
}

harpy::nest::wrappers::queue_type harpy::nest::managers::queue_manager::get_type()
{return type;
}

bool harpy::nest::managers::queue_manager::do_support_present()
{return is_support_present;}

