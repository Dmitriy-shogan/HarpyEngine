#include "vulkan_spinal_cord.h"

#include <vector>
#include <set>
#include <string>

using namespace harpy::nest;
using namespace harpy::utilities;

vulkan_spinal_cord::queue_supervisor::queue_supervisor(vulkan_spinal_cord * cord){
	this->cord = cord;
}
void vulkan_spinal_cord::queue_supervisor::init(){
	int k = 0;
	VkQueue tmp_queue;
	for (int j = 0; j < familyProperties.size(); ++j) {
		for (int i = 0; i < familyProperties[j].queueCount; ++i) {

			vkGetDeviceQueue(cord->device, j, i, &tmp_queue);
			vk_queues.push_back(tmp_queue);
			vk_queue_family.push_back(j);
			free_queues.push(k);
			k++;
		}

		//isPresentationSupported.push_back(isSupported);
	}


}
std::pair<VkQueue, uint32_t> vulkan_spinal_cord::queue_supervisor::lock_and_grab(VkQueueFlags flags){
	const std::lock_guard<std::mutex> _(lock);

	//TODO uneffective
	for (uint32_t i = 0; i < free_queues.size(); ++i) {
		uint32_t k = free_queues.front();
		free_queues.pop();
		if ((familyProperties[vk_queue_family[k]].queueFlags & flags == flags)){
			return std::make_pair(&vk_queues[k], k);
		}
		free_queues.push(k);
	}
	return std::make_pair(nullptr, -1);
}

std::pair<VkQueue, uint32_t> vulkan_spinal_cord::queue_supervisor::grab(VkQueueFlags flags){

	//TODO uneffective
	for (uint32_t i = 0; i < free_queues.size(); ++i) {
		uint32_t k = free_queues.front();
		free_queues.pop();
		if ((familyProperties[vk_queue_family[k]].queueFlags & flags == flags)){
			return std::make_pair(&vk_queues[k], k);
		}
		free_queues.push(k);
	}
	return std::make_pair(nullptr, -1);
}

std::pair<VkQueue, uint32_t> vulkan_spinal_cord::queue_supervisor::grab_presentation_queue(VkQueueFlags flags, VkSurfaceKHR surface){
	const std::lock_guard<std::mutex> _(lock);

	//TODO uneffective
	for (uint32_t i = 0; i < free_queues.size(); ++i) {
		uint32_t k = free_queues.front();
		free_queues.pop();
		if ((familyProperties[vk_queue_family[k]].queueFlags & flags == flags)){
			VkBool32 isSupported = false;
			if (vkGetPhysicalDeviceSurfaceSupportKHR(cord->ph_device, vk_queue_family[k], surface, &isSupported)!=VK_SUCCESS)throw harpy_little_error("Information about device presentation abilities is unavailable");
			if (isSupported) return std::make_pair(&vk_queues[k], k);
		}
		free_queues.push(k);
	}
	return std::make_pair(nullptr, -1);
}


void vulkan_spinal_cord::queue_supervisor::free(uint32_t index){
	const std::lock_guard<std::mutex> _(lock);
	free_queues.push(index);
}



vulkan_spinal_cord::queue_supervisor::~queue_supervisor(){
	lock.lock();
	if (free_queues.size() != vk_queues.size()) throw harpy::utilities::harpy_little_error("Trying to destruct queue_supervisor with VkQueues, which in use");
	delete &free_queues;
	delete &vk_queue_family;

	//for (int i = 0; i < vk_queues.size(); ++i) {
		//am i shouldn't destroy queues?
	//}
	delete &familyProperties;
	delete &vk_queues;
	delete &lock;
};

std::vector<VkDeviceQueueCreateInfo> * vulkan_spinal_cord::queue_supervisor::pre_init_get_queues(){


	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(cord->ph_device, &queueFamilyCount, nullptr);
	familyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(cord->ph_device, &queueFamilyCount, &familyProperties[0]);


	std::vector<VkDeviceQueueCreateInfo> logdev_queue_infos(familyProperties.size());

	for(uint32_t j = 0; j<familyProperties.size(); j++)
	{
		logdev_queue_infos[j] = {};
		logdev_queue_infos[j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		logdev_queue_infos[j].queueFamilyIndex = j;
		logdev_queue_infos[j].queueCount = familyProperties[j].queueCount;
		float queuePriority = 1.0f;
		logdev_queue_infos[j].pQueuePriorities = &queuePriority;

	}


	return &logdev_queue_infos;
}


bool vulkan_spinal_cord::check_device_extension_support() const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(ph_device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(ph_device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end()); // @suppress("Invalid template argument")

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName); // @suppress("Method cannot be resolved")
    }

    return requiredExtensions.empty(); // @suppress("Method cannot be resolved")
}

std::vector<const char*> vulkan_spinal_cord::get_required_extensions()
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

void vulkan_spinal_cord::init_instance()
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

bool vulkan_spinal_cord::is_device_suitable(VkPhysicalDevice phys_device)
{
     VkPhysicalDeviceProperties deviceProperties;
     VkPhysicalDeviceFeatures deviceFeatures;
     vkGetPhysicalDeviceProperties(phys_device, &deviceProperties);
     vkGetPhysicalDeviceFeatures(phys_device, &deviceFeatures); 

     return deviceFeatures.geometryShader;
}


void vulkan_spinal_cord::init_ph_device()
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

void vulkan_spinal_cord::init_device_and_queues()
{

    std::vector<VkDeviceQueueCreateInfo> * queueCreateInfos = queue_supervisor.pre_init_get_queues();
    

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>((*queueCreateInfos).size());
    createInfo.pQueueCreateInfos = (*queueCreateInfos).data();

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

}



vulkan_spinal_cord::~vulkan_spinal_cord()
{
    base_valid.clean_up_debug();
    delete &base_valid;
	delete &queue_supervisor;
	if (device)
	vkDestroyDevice(this->device,nullptr);
	if (instance)
	vkDestroyInstance(this->instance,nullptr);
}


void vulkan_spinal_cord::init_debug()
{
    if (!instance) throw harpy_little_error("Can't init debug before initiating instance");
    base_valid.init_debug_messenger();
}

//void vulkan_spinal_cord::connect_window(windowing::base_window_layout& win, bool do_init = true)
//{
//    connected_window_layout = win;
//}



void vulkan_spinal_cord::init_queue_supervisor(){
	this->queue_supervisor.init();
};
