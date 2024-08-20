#ifndef HARPY_NEST_WRAPPERS_RENDER_PASS
#define HARPY_NEST_WRAPPERS_RENDER_PASS
#include <nest/resources/common_vulkan_resource.h>

namespace harpy::nest::wrappers {

    class render_pass {
        VkRenderPass pass{};

        VkDevice* device{};
    public:
        render_pass(VkDevice* device = &resources::common_vulkan_resource::get_resource().get_main_device());

        void init(VkFormat format);

        VkRenderPass& get_vk_render_pass();
        operator VkRenderPass&();


        ~render_pass();
    };

}



#endif //HARPY_NEST_WRAPPERS_RENDER_PASS
