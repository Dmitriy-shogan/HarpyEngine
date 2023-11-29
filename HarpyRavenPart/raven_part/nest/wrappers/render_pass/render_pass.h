#pragma once
#ifndef HARPY_NEST_WRAPPERS_RENDER_PASS
#define HARPY_NEST_WRAPPERS_RENDER_PASS
#include <volk/volk.h>

namespace harpy::nest::wrappers
{
    //TODO: research render_pass much more
    class render_pass
    {
        VkRenderPass pass{};
    public:

        render_pass() = default;
        void init();

        VkRenderPass& get_vk_render_pass();
        operator VkRenderPass&();
        
        ~render_pass();
    };
}

#endif// HARPY_NEST_WRAPPERS_RENDER_PASS