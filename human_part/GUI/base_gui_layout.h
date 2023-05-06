#pragma once
#ifndef HARPY_GUI
#define HARPY_GUI
#include <nest/vision/base_window_layout.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_vulkan.h>

#include "vulkan_levels/soft_level_vulkan.h"

namespace harpy::gui {

    
    
    class base_gui_layout
    {
        
        constexpr static VkDescriptorPoolSize pool_sizes[]{
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        ImGuiContext* context{nullptr};
        static bool is_initialised;
    public:
        base_gui_layout(harpy_nest::soft_level_vulkan* soft_level)
        {
            using namespace ImGui;
            if (!is_initialised)
            {
                VkDescriptorPoolCreateInfo pool_create_info{};
                pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                pool_create_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                pool_create_info.maxSets = 1000;
                pool_create_info.poolSizeCount = std::size(pool_sizes);
                pool_create_info.pPoolSizes = pool_sizes;

                VkDescriptorPool pool;
                if(vkCreateDescriptorPool(soft_level->get_device(), &pool_create_info, nullptr, &pool) != VK_SUCCESS)
                {
                    throw harpy_nest::harpy_little_error(harpy_nest::error_severity::wrong_init, "Can't initialise descriptor pool for imgui");
                }
                context = CreateContext();

                ImGui_ImplGlfw_InitForVulkan(soft_level->get_base_window_layout()->get_glfw_window(), true);

                ImGui_ImplVulkan_InitInfo init_info = {};
                init_info.Instance = soft_level->get_instance();
                init_info.PhysicalDevice = soft_level->get_ph_device();
                init_info.Device = soft_level->get_device();
                init_info.Queue = soft_level->get_graphics_queue();
                init_info.DescriptorPool = pool;
                //TODO: decide whether it will be just 3 or I need to find a correct numbers
                init_info.MinImageCount = 3;
                init_info.ImageCount = 3;

                //TODO: research MSAASamples
                init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

                ImGui_ImplVulkan_Init(&init_info, soft_level->get_render_pass());

                //TODO:: research things under this
                /*immediate_submit([&](VkCommandBuffer cmd) {
                 ImGui_ImplVulkan_CreateFontsTexture(cmd);
                 });

                //clear font textures from cpu data
                ImGui_ImplVulkan_DestroyFontUploadObjects();

                //add the destroy the imgui created structures
                _mainDeletionQueue.push_function([=]() {

                    vkDestroyDescriptorPool(_device, imguiPool, nullptr);
                    ImGui_ImplVulkan_Shutdown();
                    });*/

                
                is_initialised = true;
            }
        }
    
    };
    
}

#endif //HARPY_GUI