#pragma once
#ifndef HARPY_VULKAN_WRAPPER_QUEUE
#define HARPY_VULKAN_WRAPPER_QUEUE
#include <vector>
#include <volk/volk.h>

namespace harpy::nest::wrappers
{
    
    //bit mask for queue types due to one queue can be assigned to different types
    enum queue_type
    {
        he_first = 0,
        
        graphics = VK_QUEUE_GRAPHICS_BIT,
        compute = VK_QUEUE_COMPUTE_BIT,
        transfer = VK_QUEUE_TRANSFER_BIT,
        
        main = graphics | compute,
        universal = graphics | compute | transfer,

        he_is_bm = 1,
        he_count = 6,
    };
    
    class queue_family
    {
    protected:
        uint32_t family_number{};
        std::vector<VkQueue> queues{};
        const unsigned queue_amount;
        const queue_type type;
        queue_family(queue_type q_type, uint32_t family, unsigned queue_amount)
        : family_number(family)
        , queue_amount(queue_amount)
        , type(q_type) {}
        
    public:

        virtual void init();

        queue_family& operator=(queue_family const &);
        queue_family(queue_family const &);

        queue_family& operator=(queue_family&&);
        queue_family(queue_family&&);

        virtual std::vector<VkQueue>& get_all_vk_queues();
        virtual VkQueue& get_vk_queue(size_t index);
        virtual operator VkQueue&();
        
        virtual queue_type get_type() const;
        
        virtual uint32_t get_family_index();
        virtual operator uint32_t();

        unsigned size();
        virtual ~queue_family();
    };

    
}




#endif //HARPY_VULKAN_WRAPPER_QUEUE