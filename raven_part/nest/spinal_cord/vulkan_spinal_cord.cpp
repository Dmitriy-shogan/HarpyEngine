#include "vulkan_spinal_cord.h"

#include <vector>
#include <set>
#include <string>
#include <thread>

using namespace harpy::nest;
using namespace harpy::utilities;

const std::chrono::milliseconds sleepDuration(1000);
std::shared_ptr<vulkan_spinal_cord> vulkan_spinal_cord::p_instance = nullptr;




vulkan_spinal_cord::queue_supervisor::queue_supervisor(vulkan_spinal_cord * cord){
	this->cord = cord;
}
void vulkan_spinal_cord::queue_supervisor::init(){
	int k = 0;
	VkQueue tmp_queue;
	VkCommandBuffer tmp_queue_buf;
	//pools.resize(familyProperties.size(), nullptr);
	VkCommandPool tmp;
	for (int j = 0; j < familyProperties.size(); ++j) {

		for (int i = 0; i < familyProperties[j].queueCount; ++i) {

			VkCommandPoolCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			create_info.queueFamilyIndex = j;
			create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			if (vkCreateCommandPool(cord->device, &create_info, nullptr, &tmp) != VK_SUCCESS) {
				throw utilities::harpy_little_error(utilities::error_severity::wrong_init, "failed to init command pool!");
			}
			pools.push_back(tmp);

			vkGetDeviceQueue(cord->device, j, i, &tmp_queue);
			vk_queues.push_back(tmp_queue);
			vk_queue_family.push_back(j);
			free_queues.push(k);

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.commandPool = tmp;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;

			if (vkAllocateCommandBuffers(cord->device, &allocInfo, &tmp_queue_buf) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}
			vk_queue_buffer.push_back(tmp_queue_buf);

			k++;

		}

		//isPresentationSupported.push_back(isSupported);
	}


}
std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vulkan_spinal_cord::queue_supervisor::lock_grab(VkQueueFlags flags){
	std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> res_pair;
	while (true){
		lock.lock();
		//TODO uneffective
		for (uint32_t i = 0; i < free_queues.size(); ++i) {

			uint32_t k = free_queues.front();

			free_queues.pop();




			if ((familyProperties[vk_queue_family[k]].queueFlags & flags) == flags){
				res_pair = std::make_pair(std::make_pair(vk_queues[k], vk_queue_buffer[k]), k);
				lock.unlock();

				return res_pair;
			}
			free_queues.push(k);
		}
		lock.unlock();

		std::this_thread::sleep_for(sleepDuration);
	}
	//return std::make_pair(std::make_pair(nullptr,nullptr), -1);
}

//std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vulkan_spinal_cord::queue_supervisor::grab(VkQueueFlags flags){
//
//	while(true){
//		//TODO uneffective
//		for (uint32_t i = 0; i < free_queues.size(); ++i) {
//			uint32_t k = free_queues.front();
//			free_queues.pop();
//			if ((familyProperties[vk_queue_family[k]].queueFlags & flags == flags)){
//				return std::make_pair(std::make_pair(vk_queues[k], vk_queue_buffer[k]), k);
//			}
//			free_queues.push(k);
//		}
//
//		std::this_thread::sleep_for(sleepDuration);
//	}
//	//return std::make_pair(std::make_pair(nullptr,nullptr), -1);
//}

std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vulkan_spinal_cord::queue_supervisor::grab_presentation_queue(VkQueueFlags flags, VkSurfaceKHR surface){
	std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> res_pair;
	while(true){
		lock.lock();
		//TODO uneffective
		for (uint32_t i = 0; i < free_queues.size(); ++i) {
			uint32_t k = free_queues.front();
			free_queues.pop();
			if ((familyProperties[vk_queue_family[k]].queueFlags & flags) == flags){
				VkBool32 isSupported = false;
				if (vkGetPhysicalDeviceSurfaceSupportKHR(cord->ph_device, vk_queue_family[k], surface, &isSupported)!=VK_SUCCESS)throw harpy_little_error("Information about device presentation abilities is unavailable");
				if (isSupported){
					res_pair = std::make_pair(std::make_pair(vk_queues[k], vk_queue_buffer[k]), k);
					lock.unlock();
					return res_pair;
				}
			}
			free_queues.push(k);
		}
		lock.unlock();
		std::this_thread::sleep_for(sleepDuration);
	}
	//return std::make_pair(std::make_pair(nullptr,nullptr), -1);
}


//void vulkan_spinal_cord::queue_supervisor::free(uint32_t index){
//	free_queues.push(index);
//}

void vulkan_spinal_cord::queue_supervisor::lock_free(uint32_t index){
	std::lock_guard<std::mutex> _(lock);
	free_queues.push(index);
}


vulkan_spinal_cord::queue_supervisor::~queue_supervisor(){
	lock.lock();
	if (free_queues.size() != vk_queues.size()) throw harpy::utilities::harpy_little_error("Trying to destruct queue_supervisor with VkQueues, which in use");


	for (int i = 0; i < pools.size(); ++i) {
		//am i shouldn't destroy queues?
		//if(cmd)
		vkFreeCommandBuffers(cord->device, pools[i], 1, &vk_queue_buffer[i]);
	}

	for (int j = 0; j < pools.size(); ++j) {
		vkDestroyCommandPool(cord->device, pools[j], nullptr);
	}
//	delete &free_queues;
//	delete &vk_queue_family;
//	delete &familyProperties;
//	delete &vk_queues;
//	delete &vk_queue_buffer;
//	delete &pools;
//	delete &lock;
};

std::vector<VkDeviceQueueCreateInfo> vulkan_spinal_cord::queue_supervisor::pre_init_get_queues(){


	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(cord->ph_device, &queueFamilyCount, nullptr);
	familyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(cord->ph_device, &queueFamilyCount, &familyProperties[0]);


	std::vector<VkDeviceQueueCreateInfo> logdev_queue_infos(familyProperties.size());

	priorities.resize(familyProperties.size());

	for(uint32_t j = 0; j<familyProperties.size(); j++)
	{

		priorities[j].resize(familyProperties[j].queueCount,1.0f);
		logdev_queue_infos[j] = {};
		logdev_queue_infos[j].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		logdev_queue_infos[j].queueFamilyIndex = j;
		logdev_queue_infos[j].queueCount = familyProperties[j].queueCount;
		logdev_queue_infos[j].pQueuePriorities = priorities[j].data();//&queuePriority;

	}


	return logdev_queue_infos;
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
    //uint32_t glfwExtensionCount = 0;
    //const char** glfw_extensions{nullptr};
    //glfw_extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	uint32_t cnt;
	std::vector<const char*> extentionsRequired;
	const char **glfwExts = glfwGetRequiredInstanceExtensions(&cnt);
	for (uint32_t i=0; i<cnt; i++) extentionsRequired.push_back(glfwExts[i]);

    //extensions(glfw_extensions, glfw_extensions + glfwExtensionCount);

    if constexpr (VALIDATION_LAYERS)
    {
    	extentionsRequired.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }


    return extentionsRequired;
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
    app_info.pNext=nullptr;
    VkInstanceCreateFlags create_flags{};
    
    VkInstanceCreateInfo create_info{};
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


	vkGetPhysicalDeviceProperties(ph_device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(ph_device, &deviceFeatures);
}

void vulkan_spinal_cord::init_device_and_queues()
{

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = queue_supervisor.pre_init_get_queues();
    

    VkPhysicalDeviceFeatures deviceFeatures{};
    vkGetPhysicalDeviceFeatures(ph_device, &deviceFeatures);


    VkPhysicalDeviceFeatures2 deviceFeatures2{};

	deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

	VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {};
	descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
	descriptorIndexingFeatures.pNext = nullptr;
	deviceFeatures2.pNext = &descriptorIndexingFeatures;

	vkGetPhysicalDeviceFeatures2(ph_device, &deviceFeatures2);


    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = nullptr;//&deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    createInfo.ppEnabledExtensionNames = device_extensions.data();
    createInfo.pNext = &deviceFeatures2;

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

