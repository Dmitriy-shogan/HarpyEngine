#pragma once
#ifndef HARPY_NEST_THREADING_PRIMITIVES_MEMORY_BARRIERS_FACTORY
#define HARPY_NEST_THREADING_PRIMITIVES_MEMORY_BARRIERS_FACTORY
#include <nest/initializations/inititalizations.h>
#include <volk/volk.h>


//TODO: research, some problems with understanding architecture
namespace harpy::nest::threading
{
    class memory_barriers_factory
    {
        HARPY_MAKE_SINGLETON(memory_barriers_factory)

        
    public:

        VkBufferMemoryBarrier2 create_barrier();
    
    };
}

#endif //HARPY_NEST_THREADING_PRIMITIVES_MEMORY_BARRIERS_FACTORY