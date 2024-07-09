#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVE_SEMAPHOR
#define HARPY_NEST_THREADING_PRIMITIVE_SEMAPHOR
#include <nest/resources/common_vulkan_resource.h>


namespace harpy::nest::threading
{
    //Semaphores can be used to control resource access across multiple queues.
    //GPU control
    class semaphore
    {
        bool do_delete_in_destructor = true;
        VkDevice* device{};
        VkSemaphore semaphor{};
        ;
    public:

        semaphore(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());
        
        semaphore(semaphore& sem);
        semaphore(semaphore&& sem);
        
        
        operator VkSemaphore&();
        VkSemaphore& get_semaphore();

        ~semaphore();
    
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVE_SEMAPHOR