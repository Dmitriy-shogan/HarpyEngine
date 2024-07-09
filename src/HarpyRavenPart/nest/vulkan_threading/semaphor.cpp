#include <nest/vulkan_threading/semaphor.h>

harpy::nest::threading::semaphore::semaphore(VkDevice* device) : device(device)
{
    VkSemaphoreCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    HARPY_VK_CHECK(vkCreateSemaphore(*device, &ci, nullptr, &semaphor));
}

harpy::nest::threading::semaphore::semaphore(semaphore& sem)
{
    device = sem.device;
    do_delete_in_destructor = false;
    semaphor = sem.semaphor;
}

harpy::nest::threading::semaphore::semaphore(semaphore&& sem)
{
    device = sem.device;
    semaphor = sem.semaphor;
}

harpy::nest::threading::semaphore::operator VkSemaphore_T*&()
{
    return semaphor;
}

VkSemaphore& harpy::nest::threading::semaphore::get_semaphore()
{
    return semaphor;
}

harpy::nest::threading::semaphore::~semaphore()
{
    if(semaphor && do_delete_in_destructor)
        vkDestroySemaphore(*device, semaphor, nullptr);
}

