#pragma once
#ifndef HARPY_RENDERER
#define HARPY_RENDERER
#include <swapchain/swapchain.h>
#include <interfaces/interfaces.h>
/*#include <threading/harpy_semaphore.hpp>*/
#include <utilities/harpy_little_error.h>

#include <objects/base_object.h>

#include <descriptor/descriptor.h>

#include <command_buffer_controller/command_buffer_controller.h>

#include <camera/camera_layout.h>

namespace harpy{
class renderer
{
    nest::vulkan_spinal_cord vulkan_backend{window};
    nest::swapchain chain{vulkan_backend, rend};
    nest::render_pass rend{chain};
    
    
    //TODO: Make nest part of vulkan threading
    std::vector<VkSemaphore> image_sems{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkSemaphore> finish_sems{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkFence> fences{MAX_FRAMES_IN_FLIGHT};
    std::vector<VkFence> fences_in_flight{};
    size_t frame = 0;
    
    //TEMPORARY
    nest::windowing::base_window_layout window;
    objects::base_object object{vulkan_backend, com_pool};
    nest::pipeline pipe{chain.get_render_pass()};
    nest::pools::command_pool com_pool{vulkan_backend.get_vk_device()};
    nest::pools::command_pool com_copy_pool{vulkan_backend.get_vk_device(), nest::pools::command_pool_types::copy};
    nest::pools::descriptor_pool desc_pool{vulkan_backend.get_vk_device()};
    nest::descriptor desc{vulkan_backend.get_vk_device(), desc_pool};
    std::vector<nest::buffers::command_buffer> com_bufs{MAX_FRAMES_IN_FLIGHT, {vulkan_backend.get_vk_device(), com_pool}};
    nest::command_buffer_controller controller{chain, pipe};
    nest::camera::camera_layout camera;
    
    
    
    
    
    void create_semaphores_fences();
    
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
        vulkan_backend.init();
        chain.init();
        desc.init_layout();
        pipe.init(desc);
        com_pool.init(vulkan_backend.find_queue_families(vulkan_backend.get_vk_physical_device(), vulkan_backend.get_vk_surface()));
        object.init();
        desc_pool.init();
        desc.init();
        for(auto& i :com_bufs)
        {
            i.init();
        }
        desc.populate(object.get_uniform_buffers());
        
        fences_in_flight.resize(chain.get_images().size(), nullptr);
        create_semaphores_fences();
    }
    
    void draw_frame()
    {
        vkWaitForFences(vulkan_backend.get_vk_device(), 1, &fences_in_flight[frame], VK_TRUE, UINT64_MAX);
        
        uint32_t image_index{};
        
        VkResult result = vkAcquireNextImageKHR(vulkan_backend.get_vk_device(),
                    chain, UINT64_MAX,
                    image_sems[frame], VK_NULL_HANDLE, &image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            chain.reinit();
            return;
        }
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw utilities::harpy_little_error("failed to acquire swap chain image!");
        }
        //object.rotate(15, 1, 0, 0);
        nest::ubo ub{};
        ub.model = object.get_model();
        ub.view = camera.get_view();
        ub.projection = nest::projection;
        object.get_uniform_buffers()[frame].set_ubo(ub);
        
        vkResetFences(vulkan_backend.get_vk_device(), 1, &fences_in_flight[frame]);

        vkResetCommandBuffer(com_bufs[frame], 0);
        
        controller.connect(com_bufs[frame], image_index);
        controller.draw(object.get_vertex_buffer(), object.get_index_buffer(), desc, frame);
        controller.disconnect();

        
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {image_sems[frame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &com_bufs[frame].get_vk_command_buffer();

        VkSemaphore signalSemaphores[] = {finish_sems[frame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(vulkan_backend.get_vk_present_queue(), 1, &submitInfo, fences_in_flight[frame]) != VK_SUCCESS) {
            throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {chain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &image_index;

        result = vkQueuePresentKHR(vulkan_backend.get_vk_graphics_queue(), &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.get_resize()) {
            window.get_resize() = false;
            chain.reinit();
        } else if (result != VK_SUCCESS) {
            throw utilities::harpy_little_error("failed to present swap chain image!");
        }
        
        frame = (frame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    void main_loop()
    {
        while (!glfwWindowShouldClose(vulkan_backend.get_window_layout().get_glfw_window())) {
            glfwPollEvents();
            draw_frame();
        }

        vkDeviceWaitIdle(vulkan_backend.get_vk_device());
    }

    void clean_up()
    {
        std::for_each(finish_sems.begin(), finish_sems.end(), [this](auto& x){ vkDestroySemaphore(vulkan_backend.get_vk_device(), x, nullptr);});
        std::for_each(image_sems.begin(), image_sems.end(), [this](auto& x){ vkDestroySemaphore(vulkan_backend.get_vk_device(), x, nullptr);});
        std::for_each(fences.begin(), fences.end(), [this](auto& x){ vkDestroyFence(vulkan_backend.get_vk_device(), x, nullptr);});
        std::for_each(fences_in_flight.begin(), fences_in_flight.end(), [this](auto& x){vkDestroyFence(vulkan_backend.get_vk_device(), x, nullptr);});
    }

    ~renderer()
    {
        
    }
};
}


#endif //HARPY_RENDERER