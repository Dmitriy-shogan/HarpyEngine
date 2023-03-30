#include "..//hard_level_vulkan.h"


 harpy_nest::hard_level_vulkan::needed_queues_families harpy_nest::hard_level_vulkan::find_queue_families(needed_queues_bits bits) const
 {
    needed_queues_families result{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(ph_device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(ph_device, &queueFamilyCount, queue_families.data());

    for(int i = 0; auto f : queue_families)
    {
        if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            result.graphics_families = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(ph_device, i, connected_window_layout->surface, &present_support);

        if (present_support) {
            result.present_families = i;
        }

        if (result.is_completed()) {
            break;
        }

        i++;
    }

    return result;
}

bool harpy_nest::hard_level_vulkan::check_device_extension_support() const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(ph_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(ph_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

std::vector<const char*> harpy_nest::hard_level_vulkan::get_required_extensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfw_extensions{nullptr};
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfwExtensionCount);

    if constexpr (VALIDATION_LAYERS)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void harpy_nest::hard_level_vulkan::init_instance(harpy_hard_level_settings settings)
{
    //First checking validation support
    //code goes here

    //Second info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "BASE";
    app_info.applicationVersion = APP_VERSION;
    app_info.apiVersion = API_VERSION;
    app_info.engineVersion = ENGINE_VERSION;
    app_info.pEngineName = ENGINE_NAME;

    VkInstanceCreateFlags create_flags{};
    
    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.flags = create_flags;

    const auto extensions = get_required_extensions();
    create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    create_info.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    if constexpr (VALIDATION_LAYERS)
    {
        create_info.enabledLayerCount = static_cast<uint32_t>(base_valid.layers.size());
        create_info.ppEnabledLayerNames = base_valid.layers.data();
        
        validation_layers::basic_debug_init(debug_create_info);

        create_info.pNext = &debug_create_info;
        
    } else
    {
        create_info.enabledLayerCount = 0;
        create_info.pNext = nullptr;
    }

    if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    
}

bool harpy_nest::hard_level_vulkan::is_device_suitable(VkPhysicalDevice phys_device)
{
     VkPhysicalDeviceProperties deviceProperties;
     VkPhysicalDeviceFeatures deviceFeatures;
     vkGetPhysicalDeviceProperties(phys_device, &deviceProperties);
     vkGetPhysicalDeviceFeatures(phys_device, &deviceFeatures); 

     return deviceFeatures.geometryShader;
}

void harpy_nest::hard_level_vulkan::init_ph_device(harpy_hard_level_settings settings)
{
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    if(!device_count) throw harpy_little_error("No device found. Dude, wtf?");

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for (const auto& pdevice : devices) {
        if (is_device_suitable(pdevice)) {
            ph_device = pdevice;
            break;
        }
    }

    if (ph_device == VK_NULL_HANDLE) {
        throw harpy_little_error("failed to find a suitable GPU!");
    }
}

void harpy_nest::hard_level_vulkan::init_device_and_queues(harpy_hard_level_settings settings)
{
    auto indices = find_queue_families();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphics_families.value(), indices.graphics_families.value()};

    float queuePriority = 1.0f;
    
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    createInfo.ppEnabledExtensionNames = device_extensions.data();

    if constexpr (VALIDATION_LAYERS) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(base_valid.layers.size());
        createInfo.ppEnabledLayerNames = base_valid.layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(ph_device, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    
    vkGetDeviceQueue(device, indices.present_families.value(), 0, &present_queue);
     vkGetDeviceQueue(device, indices.graphics_families.value(), 0, &graphics_queue);
}



harpy_nest::hard_level_vulkan::~hard_level_vulkan()
{
     base_valid.clean_up_debug();
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, connected_window_layout->surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void harpy_nest::hard_level_vulkan::init_default_hard()
{
     
    init_debug();
    init_ph_device();
    init_device_and_queues();
    
}

void harpy_nest::hard_level_vulkan::init_debug()
{
    if (!instance) throw harpy_little_error("Can't init debug before initiating instance");
    base_valid.init_debug_messenger();
}

void harpy_nest::hard_level_vulkan::connect_window(base_window_layout& win, bool do_init = true)
{
    connected_window_layout = &win;
    if (do_init)
    connected_window_layout->init_all(instance);
}
