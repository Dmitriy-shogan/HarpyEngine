#include <nest/vulkan_threading/fence.h>
#include <nest/resources/common_vulkan_resource.h>
#include <logger/logger.h>
using resource = harpy::nest::resources::common_vulkan_resource;


harpy::nest::threading::fence::fence(bool is_signaled, VkDevice* device) : device (device)
{
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

harpy::nest::threading::fence::fence(fence& fen)
{
    device = fen.device;
    do_delete_in_destructor = false;
    vk_fence = fen.vk_fence;
}

harpy::nest::threading::fence::fence(fence&& fen)
{
    device = fen.device;
    vk_fence = fen.vk_fence;
}


VkResult harpy::nest::threading::fence::get_status()
{
    return vkGetFenceStatus(*device, vk_fence);
}

VkFence& harpy::nest::threading::fence::get_vk_fence()
{return vk_fence;
}

harpy::nest::threading::fence::operator VkFence_T*&()
{return vk_fence;
}

VkResult harpy::nest::threading::fence::wait(std::size_t how_long_to_wait) {
    return vkWaitForFences(*device, 1, &vk_fence, true, how_long_to_wait);
}


VkResult harpy::nest::threading::fence::wait_and_reset(std::size_t how_long_to_wait)
{
    vkWaitForFences(*device, 1, &vk_fence, true, how_long_to_wait);
    return vkResetFences(*device, 1, &vk_fence);
}

VkResult harpy::nest::threading::fence::wait_for_fences(std::vector<fence> fences,
                                                        std::size_t how_long_to_wait,
                                                        VkDevice* device)
{
    std::vector<VkFence> vk_fences(fences.size());
    for(int j = 0; auto i : fences)
    {
        vk_fences[j++] = i.get_vk_fence();
    }
    return vkWaitForFences(*device, fences.size(), vk_fences.data(), true, how_long_to_wait);
}

harpy::nest::threading::fence::~fence()
{
    if(vk_fence && do_delete_in_destructor)
    {
        vkDestroyFence(*device, vk_fence, nullptr);
    }
}
