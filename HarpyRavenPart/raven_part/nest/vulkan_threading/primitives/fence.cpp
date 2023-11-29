#include "fence.h"
#include <nest/resources/common_vulkan_resource.h>
#include <utilities/logger/logger.h>
#include <nest/initializations/inititalizations.h>
using resource = harpy::nest::resources::common_vulkan_resource;

void harpy::nest::threading::primitives::fence::init(bool is_signaled = false, VkDevice* device = &resource::get_resource().get_main_device())
{
    this->device = device;

    VkFenceCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    ci.pNext = nullptr;
    if(is_signaled)
        ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    else
        ci.flags = 0;
    

    HARPY_VK_CHECK(vkCreateFence(*device,
        &ci,
        nullptr,
        &vk_fence));
}

VkResult harpy::nest::threading::primitives::fence::get_status()
{
    return vkGetFenceStatus(*device, vk_fence);
}

VkFence& harpy::nest::threading::primitives::fence::get_vk_fence()
{return vk_fence;
}

harpy::nest::threading::primitives::fence::operator VkFence_T*&()
{return vk_fence;
}

harpy::nest::threading::primitives::fence::~fence()
{
    if(vk_fence)
    {
        vkDestroyFence(*device, vk_fence, nullptr);
    }
}
