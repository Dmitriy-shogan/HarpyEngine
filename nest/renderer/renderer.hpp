#pragma once
#ifndef HARPY_RENDERER
#define HARPY_RENDERER
#include "..//vulkan_levels/soft_level_vulkan.hpp"
#include "..//interfaces/render/Idrawable.hpp"
#include <threading/harpy_semaphore.hpp>
#include <utilities/harpy_little_error.h>

namespace harpy_nest{
class renderer
{
    static std::vector<std::vector<Idrawable>> draw_objects;
    //std::vector<harpy_semaphore> semaphores;
    soft_level_vulkan vulkan_backend;

    std::vector<VkSemaphore> image_sems{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkSemaphore> finish_sems{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkFence> fences{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkFence> fences_in_flight{};
    size_t frame = 0;
    //TEMPORARY
    base_window_layout window;
    void create_semaphores_fences()
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(vulkan_backend.get_device(), &semaphoreInfo, nullptr, &image_sems[i]) != VK_SUCCESS ||
           vkCreateSemaphore(vulkan_backend.get_device(), &semaphoreInfo, nullptr, &finish_sems[i]) != VK_SUCCESS ||
           vkCreateFence(vulkan_backend.get_device(), &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS ||
           vkCreateFence(vulkan_backend.get_device(), &fenceInfo, nullptr, &fences_in_flight[i]) != VK_SUCCESS) {

                throw harpy_little_error(error_severity::wrong_init, "failed to create semaphores!");
           }
        }
    }
public:
    
    /*                      *
     *                      *
     *BLOCK FOR TESTING ONLY*
     *                      *
     */

    renderer()
    {
        vulkan_backend.init_instance();
        vulkan_backend.connect_window(window, true);
        vulkan_backend.init_default_softest();
        fences_in_flight.resize(vulkan_backend.get_swapchain_images().size(), VK_NULL_HANDLE);
    }
    void draw_frame()
    {
        create_semaphores_fences();
        uint32_t image_index{};
        vkAcquireNextImageKHR(vulkan_backend.get_device(),
                    vulkan_backend.get_swapchain(), UINT64_MAX,
                    image_sems[frame], VK_NULL_HANDLE, &image_index);

        if (fences_in_flight[image_index] != VK_NULL_HANDLE) {
            vkWaitForFences(vulkan_backend.get_device(), 1, &fences_in_flight[image_index], VK_TRUE, UINT64_MAX);
        }
        // Mark the image as now being in use by this frame
        fences_in_flight[image_index] = fences[frame];

        
        vkWaitForFences(vulkan_backend.get_device(), 1, &fences[frame], VK_TRUE, UINT64_MAX);
        
        
        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_sems[frame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vulkan_backend.get_com_buffers()[image_index];
        
        VkSemaphore signalSemaphores[] = {finish_sems[frame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        
        vkResetFences(vulkan_backend.get_device(), 1, &fences[frame]);
        if (vkQueueSubmit(vulkan_backend.get_graphics_queue(), 1, &submitInfo, fences[frame]) != VK_SUCCESS) {
            throw harpy_little_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {vulkan_backend.get_swapchain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &image_index;

        presentInfo.pResults = nullptr; // Optional
        vkQueuePresentKHR(vulkan_backend.get_present_queue(), &presentInfo);
        frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    void main_loop()
    {
        while (!glfwWindowShouldClose(&vulkan_backend.get_base_window_layout()->get_glfw_window())) {
            glfwPollEvents();
            draw_frame();
        }

        vkDeviceWaitIdle(vulkan_backend.get_device());
    }
    
    
    ~renderer()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(vulkan_backend.get_device(), finish_sems[i], nullptr);
            vkDestroySemaphore(vulkan_backend.get_device(), image_sems[i], nullptr);
            vkDestroyFence(vulkan_backend.get_device(), fences[i], nullptr);
            vkDestroyFence(vulkan_backend.get_device(), fences_in_flight[i], nullptr);
        }
    }
};
}


#endif //HARPY_RENDERER