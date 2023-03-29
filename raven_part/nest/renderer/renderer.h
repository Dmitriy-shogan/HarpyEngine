#pragma once
#ifndef HARPY_RENDERER
#define HARPY_RENDERER
#include <vulkan_levels/soft_level_vulkan.h>
#include <interfaces/render/Idrawable.h>
/*#include <threading/harpy_semaphore.hpp>*/
#include <utilities/harpy_little_error.h>
#include <primitive_data_types/vertex_buffer.h>

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
    void create_semaphores_fences();
    vertex_buffer draw_object{&vulkan_backend};
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
        draw_object.init_standart_buffer();
        vulkan_backend.record_com_buf(draw_object);
        fences_in_flight.resize(vulkan_backend.get_swapchain_images().size(), nullptr);
        create_semaphores_fences();
    }
    void draw_frame()
    {
        vkWaitForFences(vulkan_backend.get_device(), 1, &fences_in_flight[frame], VK_TRUE, UINT64_MAX);
        
        uint32_t image_index{};
        
        VkResult result = vkAcquireNextImageKHR(vulkan_backend.get_device(),
                    vulkan_backend.get_swapchain(), UINT64_MAX,
                    image_sems[frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            vulkan_backend.recreate_swapchain();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw harpy_little_error("failed to acquire swap chain image!");
        }

        vkResetFences(vulkan_backend.get_device(), 1, &fences_in_flight[frame]);

        vkResetCommandBuffer(vulkan_backend.get_com_buffers()[frame], 0);
        vulkan_backend.rec_one_com_buf(vulkan_backend.get_com_buffers()[frame], image_index, draw_object);

        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_sems[frame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vulkan_backend.get_com_buffers()[frame];

        VkSemaphore signalSemaphores[] = {finish_sems[frame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(vulkan_backend.get_present_queue(), 1, &submitInfo, fences_in_flight[frame]) != VK_SUCCESS) {
            throw harpy_little_error(error_severity::wrong_init, "failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {vulkan_backend.get_swapchain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &image_index;

        result = vkQueuePresentKHR(vulkan_backend.get_graphics_queue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.get_resize()) {
            window.get_resize() = false;
            vulkan_backend.recreate_swapchain();
        } else if (result != VK_SUCCESS) {
            throw harpy_little_error("failed to present swap chain image!");
        }
        
        frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    void main_loop()
    {
        while (!glfwWindowShouldClose(vulkan_backend.get_base_window_layout()->get_glfw_window())) {
            glfwPollEvents();
            draw_frame();
        }

        vkDeviceWaitIdle(vulkan_backend.get_device());
    }

    void clean_up()
    {
        std::ranges::for_each(finish_sems, [this](auto& x){ vkDestroySemaphore(vulkan_backend.get_device(), x, nullptr);});
        std::ranges::for_each(image_sems, [this](auto& x){ vkDestroySemaphore(vulkan_backend.get_device(), x, nullptr);});
        std::ranges::for_each(fences, [this](auto& x){ vkDestroyFence(vulkan_backend.get_device(), x, nullptr);});
        std::ranges::for_each(fences_in_flight, [this](auto& x){vkDestroyFence(vulkan_backend.get_device(), x, nullptr);});
    }
};
}


#endif //HARPY_RENDERER