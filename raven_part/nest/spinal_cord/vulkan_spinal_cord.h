#pragma once
#ifndef HARPY_HARD_VULKAN
#define HARPY_HARD_VULKAN

#include <utilities/harpy_little_error.h>
#include "validation_layers.h"
#include <utilities/utilities.h>

#include <mutex>
#include <queue>
namespace harpy::nest {

    //TODO: make transfer-only queue, if exists one
    //TODO: remove std::runtime_exception and use harpy_little_error exceptions
   
    //SINGLETON

    struct vulkan_spinal_cord
    {
//        enum needed_queues_bits
//        {
//            skip_bit = -1,
//            error_bit = 0,
//            graphic_bit = 1,
//            present_bit,
//        };
        struct queue_supervisor
        {
        	vulkan_spinal_cord * cord;
            std::mutex lock;

            std::queue<uint32_t> free_queues;
            std::vector<VkQueue> vk_queues;
            std::vector<uint32_t> vk_queue_family;


            std::vector<VkQueueFamilyProperties> familyProperties;
            //std::vector<bool> isPresentationSupported;
            std::vector<uint32_t> familyTypes= {VK_QUEUE_GRAPHICS_BIT,VK_QUEUE_TRANSFER_BIT,VK_QUEUE_COMPUTE_BIT};

        public:
            queue_supervisor(vulkan_spinal_cord * cord);
            std::vector<VkDeviceQueueCreateInfo> * pre_init_get_queues();
            void init();
            std::pair<VkQueue, uint32_t> grab(VkQueueFlags flags);
            std::pair<VkQueue, uint32_t> lock_and_grab(VkQueueFlags flags);
            std::pair<VkQueue, uint32_t> grab_presentation_queue(VkQueueFlags flags, VkSurfaceKHR surface);
            void free(uint32_t index);

            ~queue_supervisor();

        };
        
        static std::shared_ptr<vulkan_spinal_cord> p_instance;

        const std::vector<const char*> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        VkInstance instance = nullptr;
        VkPhysicalDevice ph_device = nullptr;
        VkDevice device = nullptr;

        struct queue_supervisor queue_supervisor{this};
        //TODO: create queues base class, and parallel into graphics, present and transfer AT LEAST


        validation_layers base_valid{&instance};

        

        void init_instance();
        void init_ph_device();
        void init_device_and_queues();
        void init_debug();
        void init_queue_supervisor();

        vulkan_spinal_cord(){
        	init_instance();
        	init_debug();
        	init_ph_device();
        	init_device_and_queues();
        	init_queue_supervisor();

        }
    public:
        

        static std::shared_ptr<vulkan_spinal_cord> getInstance() {
                if(!p_instance)
                    throw harpy::utilities::harpy_little_error("Failed to get spinal cord instance");
                return p_instance;
            }


        static void init()
        {
        	p_instance = std::make_shared<vulkan_spinal_cord>();
        }

        //for later
        static bool is_device_suitable(VkPhysicalDevice phys_device);


        bool check_device_extension_support() const;

        static std::vector<const char*> get_required_extensions();
        VkDevice& get_vk_device(){return device;}
        VkPhysicalDevice& get_vk_physical_device(){return ph_device;}
        VkInstance& get_vk_instance(){return instance;}
        //VkSurfaceKHR& get_vk_surface(){return connected_window_layout.get_VK_surface();}
        //nest::swapchain& get_vk_swapchain(){return swapchain;}
        struct queue_supervisor& get_queue_supervisor(){return queue_supervisor;}
        //windowing::base_window_layout& get_window_layout(){return connected_window_layout;}

        ~vulkan_spinal_cord();
    };
    
}

#endif
