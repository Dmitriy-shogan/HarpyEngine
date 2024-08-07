#include <nest/resources/common_vulkan_resource.h>
#include <logger/harpy_little_error.h>
#include <nest/managers/swapchain_manager.h>

using namespace harpy::nest;
using namespace resources;

#define INIT_CHECK(x) if(is_instance_initialized) \
    return x; \
    throw harpy::utilities::harpy_little_error \
    (harpy::utilities::error_severity::not_init, "Common vulkan resource is not initialized yet")

//USE THIS FOR INITIALIZING INSTANCE
VkInstance& common_vulkan_resource::get_instance()
{
    is_instance_initialized = true;
    return instance;
}

common_vulkan_resource::operator VkInstance_T*&()
{
    INIT_CHECK(instance);
}

std::vector<VkSurfaceKHR>& common_vulkan_resource::get_all_surfaces()
{
    INIT_CHECK(surfaces);
}

VkSurfaceKHR& common_vulkan_resource::get_main_surface()
{
    INIT_CHECK(surfaces.front());
}

common_vulkan_resource::operator VkSurfaceKHR_T*&()
{
    INIT_CHECK(surfaces.front());
}

VkDebugUtilsMessengerEXT& common_vulkan_resource::get_debug_messenger()
{return debug_messenger;}

common_vulkan_resource::operator VkDebugUtilsMessengerEXT_T*&()
{return debug_messenger;}

VkPhysicalDevice& common_vulkan_resource::get_phys_device()
{INIT_CHECK(phys_device);}

common_vulkan_resource::operator VkPhysicalDevice_T*&()
{INIT_CHECK(phys_device);}

VkPhysicalDeviceFeatures2& common_vulkan_resource::get_phys_features()
{INIT_CHECK(phys_features);
}

common_vulkan_resource::operator VkPhysicalDeviceFeatures2&()
{INIT_CHECK(phys_features);
}

VkPhysicalDeviceProperties2& common_vulkan_resource::get_phys_props()
{INIT_CHECK(phys_props);
}

common_vulkan_resource::operator VkPhysicalDeviceProperties2&()
{INIT_CHECK(phys_props);
}

std::vector<VkDevice>& common_vulkan_resource::get_all_devices()
{INIT_CHECK(devices);}

VkDevice& common_vulkan_resource::get_main_device()
{
    if(!is_inited()) throw utilities::harpy_little_error
    (utilities::error_severity::not_init, "Common vulkan resource is not initialized yet");
    return devices.front();
}

common_vulkan_resource::operator VkDevice_T*&()
{
    if(!is_inited()) throw utilities::harpy_little_error
    (utilities::error_severity::not_init, "Common vulkan resource is not initialized yet");
    return devices.front();
}

std::vector<windowing::window>& common_vulkan_resource::get_all_windows()
{INIT_CHECK(windows);}

windowing::window& common_vulkan_resource::get_main_window()
{
    if(!is_inited()) throw utilities::harpy_little_error
    (utilities::error_severity::not_init, "Common vulkan resource is not initialized yet");
    return windows.front();
}

common_vulkan_resource::operator harpy::nest::windowing::window&()
{
    if(!is_inited()) throw utilities::harpy_little_error
   (utilities::error_severity::not_init, "Common vulkan resource is not initialized yet");
    return windows.front();
}

void common_vulkan_resource::set_main_window(windowing::window_create_info win_ci)
{
    if(windows.empty())
    {
        windows.push_back(windowing::window(win_ci));
        return;
    }
    windows[0] = windowing::window(win_ci);
}

void common_vulkan_resource::set_main_window(windowing::window&& win)
{
    if(windows.empty())
    {
        windows.emplace_back(std::move(win));
        return;
    }
    windows[0] = std::move(win);
}

std::size_t common_vulkan_resource::add_window(windowing::window_create_info win_ci)
{
    windows.emplace_back(win_ci);
    
    return windows.size() - 1;
}

std::size_t common_vulkan_resource::add_window(windowing::window&& win)
{
    windows.emplace_back(std::move(win));
    return windows.size() - 1;
}


std::vector<wrappers::queue_family>& common_vulkan_resource::get_all_family_queues()
{INIT_CHECK(family_queues);}

wrappers::queue_family& common_vulkan_resource::get_main_family_queue()
{return family_queues[0];}

std::vector<VmaAllocator>& common_vulkan_resource::get_allocators()
{
    INIT_CHECK(allocators);
}

VmaAllocator& common_vulkan_resource::get_main_allocator()
{ INIT_CHECK(allocators.front());}

common_vulkan_resource::operator VmaAllocator_T*&()
{INIT_CHECK(allocators.front());}

bool common_vulkan_resource::is_inited()
{return is_instance_initialized && !devices.empty();}

common_vulkan_resource& common_vulkan_resource::get_resource()
{
    static common_vulkan_resource res{};
    return res;
}

common_vulkan_resource::~common_vulkan_resource()
{
    for(auto& i : allocators)
    {
        if (i) vmaDestroyAllocator(i);
    }
    for(auto& i : devices)
    {
        if (i) vkDestroyDevice(i, nullptr);
    }
    
    if (debug_messenger)
    {
        if (!vkDestroyDebugUtilsMessengerEXT)
        {
            PFN_vkDestroyDebugUtilsMessengerEXT myvkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>( vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" ) );
            myvkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
        }
        else
        {
            vkDestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
        }
    }
    
    for(auto& i : windows)
    {
        i.~window();
        i.get_glfw_window() = nullptr;
    }
    
    for(auto& i : surfaces)
    {
        if (i) vkDestroySurfaceKHR(instance, i, nullptr);
    }
    
    if (instance) vkDestroyInstance(instance, nullptr);
            
    //ENSURE to destroy windows BEFORE terminating gltf
    
    glfwTerminate();
}

