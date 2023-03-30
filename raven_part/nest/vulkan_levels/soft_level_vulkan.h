#pragma once
#ifndef HARPY_SOFT_VULKAN
#define HARPY_SOFT_VULKAN
#include <vulkan_levels/medium_level_vulkan.h>
#include <utilities/utilities.h>
#include <shaders/base_shader.h>
#include <com_pools_and_buffers/cp_pool.h>

#include "primitive_data_types/buffers/index_buffer.h"

namespace harpy_nest{
    class vertex_buffer;
    
    //TODO: everything from soft_level must be in different parts of nest
class soft_level_vulkan : public medium_level_vulkan
{
    VkPipelineLayout pipeline_layout{};
    VkRenderPass render_pass{};
    VkPipeline graphics_pipeline{};
    std::vector<VkFramebuffer> framebuffers_array;

    std::unique_ptr<cp_pool> pool;
    
    
    std::vector<VkCommandBuffer> com_buffers;
    
    void init_graphics_pipeline();
    void init_render_pass();
    void init_framebuffers();
    void init_com_buffers();
    void clean_up_swapchain();

    void reinit_swapchain();

public:

    void rec_one_com_buf(VkCommandBuffer buffer, uint32_t image_index, vertex_buffer& buf);
    void rec_one_com_buf(VkCommandBuffer buffer, uint32_t image_index, vertex_buffer& buf, index_buffer& indexes);
    void record_com_buf(vertex_buffer& buf);
    void record_com_buf(vertex_buffer& buf, index_buffer& indexes);
    
    soft_level_vulkan() : medium_level_vulkan(){}
    void init_default_softest()
    {
        init_default_softer();
        init_render_pass();
        init_graphics_pipeline();
        init_framebuffers();
        pool = std::make_unique<cp_pool>(this);
        init_com_buffers();
    }


    ~soft_level_vulkan() override
    {
        
        for (auto framebuffer : framebuffers_array) {
            vkDestroyFramebuffer(device, framebuffer, nullptr);
        }
        vkDestroyPipeline(device, graphics_pipeline, nullptr);
        vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
        vkDestroyRenderPass(device, render_pass, nullptr);
    }

    VkSwapchainKHR get_swapchain() const {return swapchain;}
    std::vector<VkCommandBuffer>& get_com_buffers(){return com_buffers;}
    VkQueue get_graph_queue() const {return graphics_queue;}
    VkQueue get_present_queue() const {return present_queue;}
    VkInstance get_instance() const;
    VkPhysicalDevice get_ph_device() const;
    VkDevice& get_device() ;
    VkQueue get_graphics_queue() const;
    validation_layers& get_valid_layers();
    base_window_layout* get_base_window_layout() const;
    std::vector<VkImage> get_swapchain_images() const {return swapchain_images;}
    VkRenderPass& get_render_pass()  { return render_pass; }
    cp_pool& get_pools(){return *pool;}

    void recreate_swapchain()
    {
        vkDeviceWaitIdle(device);

        int width = 0, height = 0;
        glfwGetFramebufferSize(connected_window_layout->get_glfw_window(), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(connected_window_layout->get_glfw_window(), &width, &height);
            glfwWaitEvents();
        }

        reinit_swapchain();
        init_image_views();
        init_framebuffers();
    }

    
    

    const std::vector<const char*>& get_device_extensions() const;
};

}



#endif // HARPY_SOFT_VULKAN