#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#include <nest/inititalizations.h>

#include <iostream>
#include <nest/resources/common_vulkan_resource.h>

#include <GLFW/glfw3.h>


#include <logger/logger.h>
#include <vector>
#include <array>

#include <nest/resources/surface_capabilities.h>

#include "nest/managers/queue_manager.h"

using logger = harpy::utilities::error_handling::logger;
using resource = harpy::nest::resources::common_vulkan_resource;

using namespace harpy;
using namespace nest;

void init_vk_instance()
{
    logger::get_logger() << "Hello, fellow programmers. Starting initiating harpy resources" << "Initializing volk";
    HARPY_VK_CHECK(volkInitialize());

    //Here initiating instance
    logger::get_logger() << "Starting initializing instance";

    //First app info
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Just testing";
    app_info.applicationVersion = APP_VERSION;
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = ENGINE_VERSION;
    app_info.apiVersion = VK_API_VERSION_1_2;

    if constexpr (is_harpy_debug)
    {
        logger::get_logger() << "Some debug info: "
        << "Application name: " " Just testing" 
        << "Application version: "  
        << "Api version: " 
        << "Engine name: " ENGINE_NAME 
        << "Engine version: ";
    }
    
    VkInstanceCreateInfo instance_ci{};
    instance_ci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_ci.pApplicationInfo = &app_info;

    uint32_t extension_count = 0;
    const char** GLTW_extentions {glfwGetRequiredInstanceExtensions(&extension_count)};
    
    std::vector<const char*> extensions{};
    extensions.resize(extension_count);
    for(uint32_t i = 0; i < extension_count; i++)
    {
        extensions.push_back(GLTW_extentions[i]);
    }
    
    VkDebugUtilsMessengerCreateInfoEXT debug_ci{};
    if constexpr (is_validation_layers)
    {
        logger::get_logger() << "Starting validation layers";
        bool is_val_layer {false};
        uint32_t layer_count;
        vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
        std::vector<VkLayerProperties> layer_propertieses{layer_count};
        HARPY_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, layer_propertieses.data()));

        for(auto& i : resources::validation_layers)
        {
            for(auto& f : layer_propertieses)
            {
                if (strcmp(i, f.layerName) == 0) {
                    is_val_layer = true;
                    goto stop;
                }
            }
        }
        //HERE GOES STOP
        stop:
        //HERE GOES STOP
        if(!is_val_layer) throw harpy::utilities::error_handling::harpy_little_error("No validation layers found");
        
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        
        instance_ci.ppEnabledLayerNames = resources::validation_layers.data();
        instance_ci.enabledLayerCount = static_cast<uint32_t>(resources::validation_layers.size());

        debug_ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_ci.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_ci.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_ci.pfnUserCallback = debugCallback;

        instance_ci.pNext = &debug_ci;
        
        logger::get_logger() << "Validation layers have been enabled, wait till instance will be loaded";
    }
    else
    {
        instance_ci.pNext = nullptr;
        instance_ci.enabledLayerCount = 0;
    }

    std::vector<const char*> working_extensions;
    for(auto& i : extensions)
    {
        if(i) working_extensions.push_back(i);
    }

    instance_ci.enabledExtensionCount = working_extensions.size();
    instance_ci.ppEnabledExtensionNames = working_extensions.data();

    HARPY_VK_CHECK(vkCreateInstance(&instance_ci, nullptr, &resource::get_resource().get_instance()));

    logger::get_logger() << "Successfully created instance, pushing it to volk";

    
    volkLoadInstance(resource::get_resource());
    logger::get_logger() << "Successfully pushed to volk, initiating validation layers (if they exists)";

    if constexpr(is_harpy_debug)
    {
        logger::get_logger() << "Instance check!";
        logger::get_logger() << std::to_string(reinterpret_cast<std::intptr_t>(resource::get_resource().get_instance()));
    }
    
    if constexpr(is_validation_layers)
    {
        if (!vkCreateDebugUtilsMessengerEXT)
        {
            PFN_vkCreateDebugUtilsMessengerEXT myvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>( vkGetInstanceProcAddr( resource::get_resource(), "vkCreateDebugUtilsMessengerEXT" ) );
            HARPY_VK_CHECK(myvkCreateDebugUtilsMessengerEXT(resource::get_resource().get_instance(), &debug_ci,
            nullptr, &resource::get_resource().get_debug_messenger()));
        }
        else
        {
            HARPY_VK_CHECK(vkCreateDebugUtilsMessengerEXT(resource::get_resource().get_instance(), &debug_ci,
                nullptr, &resource::get_resource().get_debug_messenger()));
        }
        
        logger::get_logger() << "Succesfully installed validation layers";
    }
    if constexpr (is_harpy_debug)
    {
        logger::get_logger() << "Some debug info: ";
        uint32_t extentions_count{};
        
        vkEnumerateInstanceExtensionProperties(nullptr, &extentions_count, nullptr);
        std::vector<VkExtensionProperties> device_extentions{extentions_count};
        vkEnumerateInstanceExtensionProperties(nullptr, &extentions_count, device_extentions.data());

        logger::get_logger() << "Available extensions:";

        for(auto& i : device_extentions)
        {
            logger::get_logger() << i.extensionName;
        }
        logger::get_logger() << "That's all for extensions!";
    }
}
void init_window()
{
    logger::get_logger() << "Starting windows creation";

    //Window part.
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    //Disabling till swapchain
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    resource::get_resource().set_main_window(windowing::window({800, 600, false, false, "Testing"}));
    
    logger::get_logger() << "Window created succesfully, gonna create physical device";
}
void init_vk_physical_device()
{
    uint32_t phys_device_counter{};
    vkEnumeratePhysicalDevices(resource::get_resource(), &phys_device_counter, nullptr);

    if(!phys_device_counter) throw utilities::error_handling::harpy_little_error("No physical devices found, man. Buy a graphics card!");
    
    std::vector<VkPhysicalDevice> devices{phys_device_counter};
    vkEnumeratePhysicalDevices(resource::get_resource(), &phys_device_counter, devices.data());

    logger::get_logger() << "Found " + std::to_string(phys_device_counter) + ". Starting searching ideal.";

    //TODO: when we need features: two arrays: обязательный и дополнительный (при отсутвии фич из первого, выкидываем ошибку)
    VkPhysicalDeviceFeatures2 features{};
    features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            
    VkPhysicalDeviceProperties2 props{};
    props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

    //Block for deciding graph card.
    {
        if(phys_device_counter == 1)
        {
            resource::get_resource().get_phys_device() = devices.front();
            
            
            vkGetPhysicalDeviceFeatures2(resource::get_resource(), &features);
            vkGetPhysicalDeviceProperties2(resource::get_resource(), &props);

            resource::get_resource().get_phys_features() = features;
            resource::get_resource().get_phys_props() = props;

            initializations::vendor = static_cast<initializations::vendors_id>(props.properties.vendorID);
            if constexpr(is_harpy_debug)
            {
                logger::get_logger() << "Some debug info:"
                << "Device ID:" + std::to_string(props.properties.deviceID)
                << "Vendor ID:" + std::to_string(props.properties.vendorID)
                << "Device type (enum value):" + std::to_string(props.properties.deviceType)
                << "Driver version:" + std::to_string(props.properties.driverVersion)
                << "Device name:" + std::string(props.properties.deviceName);
            }
        }
        
        for(auto& i : devices)
        {
            
            vkGetPhysicalDeviceFeatures2(i, &features);
            vkGetPhysicalDeviceProperties2(i, &props);

            if(props.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                initializations::vendor = static_cast<initializations::vendors_id>(props.properties.vendorID);
                resource::get_resource().get_phys_device() = devices.front();
                resource::get_resource().get_phys_features() = features;
                resource::get_resource().get_phys_props() = props;
                logger::get_logger() << "Succesfully chosen a gpu.";
                if constexpr(is_harpy_debug)
                {
                    logger::get_logger() << "Some debug info:"
                    << "Device ID:" + std::to_string(props.properties.deviceID)
                    << "Vendor ID:" + std::to_string(props.properties.vendorID)
                    << "Device type (enum value):" + std::to_string(props.properties.deviceType)
                    << "Driver version:" + std::to_string(props.properties.driverVersion)
                    << "Device name:" + std::string(props.properties.deviceName);
                }
                break;
            }
        }
    } //Block end
    
}
void init_vk_logical_device()
{
    logger::get_logger() << "Starting creating queues";
    
    std::vector<uint32_t> queue_amount_to_create{};
    
    uint32_t queue_families_amount{};
    vkGetPhysicalDeviceQueueFamilyProperties2(resource::get_resource(), &queue_families_amount, nullptr);
    
    std::vector<VkQueueFamilyProperties2> families{queue_families_amount};
    for(auto& i : families)
    {
        i.sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
    }
    
    vkGetPhysicalDeviceQueueFamilyProperties2(resource::get_resource(), &queue_families_amount, families.data());

    resource::get_resource().get_all_family_queues().reserve(queue_families_amount);
    if(queue_families_amount == 1)
    {
        logger::get_logger() << "We found only one queue type, so here goes universal family queue";
        resource::get_resource().get_all_family_queues().emplace_back(wrappers::queue_family{wrappers::queue_type::universal, 0, 1});
        queue_amount_to_create.push_back(1);
    }
    else
    {
        logger::get_logger() << "Found more than one queue family. Good.";
        unsigned flags{};

        for(uint32_t f = 0; auto& i : families)
        {
            if(queue_families_amount == 2
                && !(flags & wrappers::queue_type::main)
                && (i.queueFamilyProperties.queueFlags & (VK_QUEUE_GRAPHICS_BIT & VK_QUEUE_COMPUTE_BIT)))
            {
                resource::get_resource().get_all_family_queues().emplace_back(wrappers::queue_type::main, f++, i.queueFamilyProperties.queueCount);
                flags |= wrappers::main;
                queue_amount_to_create.push_back(1);
                continue;
            }
            if(i.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                resource::get_resource().get_all_family_queues().emplace_back(wrappers::queue_type::graphics, f++, i.queueFamilyProperties.queueCount);
                flags |= wrappers::graphics;
                queue_amount_to_create.push_back(1);
                continue;
            }
            if (i.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                resource::get_resource().get_all_family_queues().emplace_back(wrappers::queue_type::compute, f++, i.queueFamilyProperties.queueCount);
                flags |= wrappers::compute;
                queue_amount_to_create.push_back(i.queueFamilyProperties.queueCount);
                continue;
            }
            if (i.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                resource::get_resource().get_all_family_queues().emplace_back(wrappers::queue_type::transfer, f++, i.queueFamilyProperties.queueCount);
                flags |= wrappers::transfer;
                queue_amount_to_create.push_back(i.queueFamilyProperties.queueCount);
                continue;
            }
        }
    }
    
    logger::get_logger() << "Succesfully established vector of queues. Now creating logical device and queues";

    const float priority = 1.0f;

    resource::get_resource().get_all_devices().emplace_back();
    resource::get_resource().get_phys_features().sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(resource::get_resource(), &resource::get_resource().get_phys_features());

    VkDeviceCreateInfo device_ci{};
    device_ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_ci.pNext = &resource::get_resource().get_phys_features();
    device_ci.queueCreateInfoCount = families.size();

    //Due to RANDOM MAGIC std::array<VkDeviceQueueCreateInfo, families.size()> doesn't work in Rider. I dunno if it's only in Rider
    VkDeviceQueueCreateInfo* queues_ci = new VkDeviceQueueCreateInfo[families.size()];
    device_ci.pQueueCreateInfos = queues_ci;

    for(int i = 0; i < families.size(); i++)
    {
        queues_ci[i] = VkDeviceQueueCreateInfo{};
        queues_ci[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queues_ci[i].queueCount = queue_amount_to_create[i];
        queues_ci[i].queueFamilyIndex = resource::get_resource().get_all_family_queues()[i].get_family_index();
        queues_ci[i].pQueuePriorities = &priority;
    }

    //Check for swapchain supports
    {
        uint32_t device_extention_count {};
        vkEnumerateDeviceExtensionProperties(resource::get_resource().get_phys_device(), nullptr, &device_extention_count, nullptr);
        
        std::vector<VkExtensionProperties> props{device_extention_count};
        vkEnumerateDeviceExtensionProperties(resource::get_resource().get_phys_device(), nullptr, &device_extention_count, props.data());
        
        bool is_swapchain{false};
        for(auto& i : props)
        {
            if (std::string(i.extensionName) == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME))
            {
                is_swapchain = true;
                break;
            }
        }
        if(!is_swapchain)
            throw utilities::error_handling::harpy_little_error("No swapchain extension has been found. No way you can use this engine");
    }

    device_ci.enabledExtensionCount = static_cast<uint32_t>(resources::musthave_device_extentions.size());
    device_ci.ppEnabledExtensionNames = resources::musthave_device_extentions.data(); 

    if constexpr  (is_validation_layers) {
        device_ci.enabledLayerCount = static_cast<uint32_t>(resources::validation_layers.size());
        device_ci.ppEnabledLayerNames = resources::validation_layers.data();
    } else {
        device_ci.enabledLayerCount = 0;
    }

    HARPY_VK_CHECK(vkCreateDevice(resource::get_resource(),
        &device_ci,
        nullptr,
        &resource::get_resource().get_main_device()));

    delete[] queues_ci;
    
    logger::get_logger() << "Succesfully created logical device";
}
void init_vk_surface()
{
    resource::get_resource().get_all_surfaces().emplace_back();

    glfwCreateWindowSurface(resource::get_resource(),
        resource::get_resource().get_main_window(),
        nullptr,
        &resource::get_resource().get_main_surface());
    
    logger::get_logger() << "Successfully created surface.";
}
void init_vma()
{
    logger::get_logger() << "Starting creating VMA";
    resource::get_resource().get_allocators().emplace_back(nullptr);
    
    VmaVulkanFunctions vma_vulkan_func{};
    vma_vulkan_func.vkAllocateMemory                    = vkAllocateMemory;
    vma_vulkan_func.vkBindBufferMemory                  = vkBindBufferMemory;
    vma_vulkan_func.vkBindImageMemory                   = vkBindImageMemory;
    vma_vulkan_func.vkCreateBuffer                      = vkCreateBuffer;
    vma_vulkan_func.vkCreateImage                       = vkCreateImage;
    vma_vulkan_func.vkDestroyBuffer                     = vkDestroyBuffer;
    vma_vulkan_func.vkDestroyImage                      = vkDestroyImage;
    vma_vulkan_func.vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
    vma_vulkan_func.vkFreeMemory                        = vkFreeMemory;
    vma_vulkan_func.vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
    vma_vulkan_func.vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
    vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
    vma_vulkan_func.vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
    vma_vulkan_func.vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
    vma_vulkan_func.vkMapMemory                         = vkMapMemory;
    vma_vulkan_func.vkUnmapMemory                       = vkUnmapMemory;
    vma_vulkan_func.vkCmdCopyBuffer                     = vkCmdCopyBuffer;

     
    VmaAllocatorCreateInfo allocator_ci = {};
    //Just for now
    allocator_ci.vulkanApiVersion = VK_API_VERSION_1_0;
    allocator_ci.physicalDevice = resource::get_resource();
    allocator_ci.device = resource::get_resource();
    allocator_ci.instance = resource::get_resource();
    allocator_ci.pVulkanFunctions = &vma_vulkan_func;
 
    vmaCreateAllocator(&allocator_ci, &resource::get_resource().get_main_allocator());
    logger::get_logger() << "Succesfully created VMA";
}



void harpy::nest::initializations::init_vk_common_resource()
{
    init_vk_instance();
    init_window();
    init_vk_physical_device();
    init_vk_logical_device();
    init_vk_surface();
    init_vma();
    //TODO: populate debug info into lower functions
    
    logger::get_logger() << "Your common vulkan resources have been created succesfully. Now you can use other vulkan features in Harpy Engine";
}

void harpy::nest::initializations::init_harpy()
{
    glfwInit();
    init_vk_common_resource();
    init_std_surface_capabilities();
}

void initializations::init_std_surface_capabilities()
{
    resources::std_surface_capabilities = resources::create_surface_capabilities();
}

