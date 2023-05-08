#include "render_pass.h"
#include <swapchain/swapchain.h>


harpy::nest::render_pass::render_pass(swapchain& chain): device(chain.get_vk_device()), format(chain.get_vk_surface_format().format)
{}
