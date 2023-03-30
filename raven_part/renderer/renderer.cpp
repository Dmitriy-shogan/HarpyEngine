#include "renderer.h"
void harpy_renderer::renderer::create_semaphores_fences()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(vulkan_backend.get_device(), &semaphoreInfo, nullptr, &image_sems[i]) != VK_SUCCESS ||
            vkCreateSemaphore(vulkan_backend.get_device(), &semaphoreInfo, nullptr, &finish_sems[i]) != VK_SUCCESS ||
            vkCreateFence(vulkan_backend.get_device(), &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {

            throw harpy_little_error(error_severity::wrong_init, "failed to create semaphores!");
        }
    }
    for(auto& i : fences_in_flight)
    {
        if(vkCreateFence(vulkan_backend.get_device(), &fenceInfo, nullptr, &i) != VK_SUCCESS) {
            throw harpy_little_error(error_severity::wrong_init, "failed to create semaphores!");
        }
    }
}

