#pragma once
#ifndef HARPY_NEST_MANAGERS_QUEUE
#define HARPY_NEST_MANAGERS_QUEUE
#include <deque>
#include <nest/inititalizations.h>
#include "nest/wrappers/queue_family.h"

namespace harpy::nest::managers
{
    class queue_manager;
    
    class queue_wrapper
    {
        queue_manager* manager{};
        wrappers::queue_type type;
        VkQueue queue;
        queue_wrapper(VkQueue queue, wrappers::queue_type type);

    public: 

        VkQueue& get_queue();
        wrappers::queue_type get_type();
        explicit operator wrappers::queue_type const &  () const;
        operator VkQueue const &  () const;
        operator VkQueue const *  () const;

        ~queue_wrapper();
        friend class queue_manager;
    };
    
    class queue_manager
    {
        std::deque<VkQueue> queues;
        uint32_t family_index{};
        wrappers::queue_type type{};
        bool is_support_present{};
        
        friend class queue_wrapper;
    public:
        queue_manager(wrappers::queue_family& family);
        
        queue_wrapper get_queue();
        std::deque<VkQueue>& get_all_available_queues();

        wrappers::queue_type get_type();

        bool do_support_present();
    };
}

#endif //HARPY_NEST_MANAGERS_QUEUE