#pragma once
#ifndef HARPY_SOFT_VULKAN
#define HARPY_SOFT_VULKAN
#include <vulkan_levels/medium_level_vulkan.h>
#include <utilities/utilities.h>
#include <shaders/base_shader.h>

namespace harpy::nest{
    
    //TODO: everything from soft_level must be in different parts of nest
class soft_level_vulkan : public medium_level_vulkan
{

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


    virtual ~soft_level_vulkan() {}

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