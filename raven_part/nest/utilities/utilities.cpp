#include "utilities.h"
#include <utilities/harpy_little_error.h>

std::vector<char> harpy::utilities::read_file(std::string filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);
    std::vector<char> res(file.tellg());
    file.seekg(0);
    file.read(res.data(), res.size());
    file.close();
    return res;
}

std::ostream& harpy::utilities::operator<<(std::ostream& out, nest::vertex& vertex)
{
    out << "Coordinata: " << std::to_string(vertex.position.x) + " " + std::to_string(vertex.position.y) + " " <<std::endl;
    out << "Color: " << std::to_string(vertex.color.x) + " " + std::to_string(vertex.color.y) + " " + std::to_string(vertex.color.z) << std::endl;
    return out;
}

uint32_t harpy::utilities::find_memory_types(VkPhysicalDevice& device, uint32_t typeFilter,
                                             VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw harpy_little_error("failed to find suitable memory type!");
}

harpy::nest::needed_queues_families harpy::utilities::find_queue_families(VkPhysicalDevice& ph_device,
    VkSurfaceKHR& surface)
{
    nest::needed_queues_families result{};

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
        vkGetPhysicalDeviceSurfaceSupportKHR(ph_device, i, surface, &present_support);

        if (present_support) {
            result.present_families = i;
        }

        if(f.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            result.transfer_families = i;
        }

        if (result.is_completed()) {
            break;
        }

        i++;
    }

    return result;
}
