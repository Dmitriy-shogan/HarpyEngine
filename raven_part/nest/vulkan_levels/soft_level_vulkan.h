#pragma once
#ifndef HARPY_SOFT_VULKAN
#define HARPY_SOFT_VULKAN
#include <vulkan_levels/medium_level_vulkan.h>
#include <utilities/utilities.h>
#include <shaders/base_shader.h>

namespace harpy_nest{
    class vertex_buffer;

    class renderer;
    //TODO: everything from soft_level must be in different parts of nest
class soft_level_vulkan : public medium_level_vulkan
{
    VkPipelineLayout pipeline_layout{};
    VkRenderPass render_pass{};
    VkPipeline graphics_pipeline{};
    std::vector<VkFramebuffer> framebuffers_array;

    VkCommandPool draw_com_pool{};
    VkCommandPool copy_com_pool{};
    std::vector<VkCommandBuffer> com_buffers;
    
    void init_graphics_pipeline();
    void init_render_pass();
    void init_framebuffers();
    void init_com_pool();
    void init_com_buffers();
    void clean_up_swapchain();

    void reinit_swapchain();

public:

    void rec_one_com_buf(VkCommandBuffer buffer, uint32_t image_index, vertex_buffer& buf);
    void record_com_buf(vertex_buffer& buf);
    
    soft_level_vulkan() : medium_level_vulkan(){}
    void init_default_softest()
    {
        init_default_softer();
        init_render_pass();
        init_graphics_pipeline();
        init_framebuffers();
        init_com_pool();
        init_com_buffers();
    }


    ~soft_level_vulkan() override
    {
        vkDestroyCommandPool(device, draw_com_pool, nullptr);
        vkDestroyCommandPool(device, copy_com_pool, nullptr);
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
    VkCommandPool& get_copy_pool()  {return copy_com_pool;}

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