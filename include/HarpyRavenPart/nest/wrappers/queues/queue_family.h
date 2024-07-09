#pragma once
#ifndef HARPY_VULKAN_WRAPPER_QUEUE
#define HARPY_VULKAN_WRAPPER_QUEUE
#include <volk.h>

namespace harpy::nest::wrappers
{
    
    //bit mask for queue types due to one queue can be assigned to different types
    enum queue_type : uint16_t
    {
        
        
        graphics = VK_QUEUE_GRAPHICS_BIT,
        compute = VK_QUEUE_COMPUTE_BIT,
        transfer = VK_QUEUE_TRANSFER_BIT,
        
        main = graphics | compute,
        universal = graphics | compute | transfer,

        he_first = graphics,
        he_is_bm = 1,
        he_count = 5,
    };
    
    class queue_family
    {
    protected:
        uint32_t family_number{};
        const queue_type type;
        const unsigned queue_amount;
    
    public:
        queue_family(queue_type q_type, uint32_t family, unsigned queue_amount);

        virtual queue_type get_type() const;
        
        virtual uint32_t get_family_index();
        virtual operator uint32_t();

        virtual bool do_support_presentation();

        unsigned size();
        virtual ~queue_family();
    };

    
}




#endif //HARPY_VULKAN_WRAPPER_QUEUE