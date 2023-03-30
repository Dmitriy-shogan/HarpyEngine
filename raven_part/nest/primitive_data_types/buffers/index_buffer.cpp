#include "index_buffer.h"
#include <vulkan_levels/soft_level_vulkan.h>

void harpy_nest::index_buffer::init_index_buffer()
{VkBufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create_info.size = sizeof(indices[0]) * indices.size();
    create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer stage_buf{nullptr};

    if(vkCreateBuffer(vulkan_backend->get_device(), &create_info, nullptr, &stage_buf))
        throw harpy_little_error(error_severity::wrong_init, "Staging buffer was not created succesfully");

    create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    if(vkCreateBuffer(vulkan_backend->get_device(), &create_info, nullptr, &index_buf))
        throw harpy_little_error(error_severity::wrong_init, "Vertex buffer was not created succesfully");

            
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(vulkan_backend->get_device(), index_buf, &mem_requirements);

    VkMemoryRequirements stage_mem_req;
    vkGetBufferMemoryRequirements(vulkan_backend->get_device(), stage_buf, &stage_mem_req);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = stage_mem_req.size;
    alloc_info.memoryTypeIndex = find_memory_type(stage_mem_req.memoryTypeBits,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDeviceMemory stage_mem;

    if (vkAllocateMemory(vulkan_backend->get_device(), &alloc_info, nullptr, &stage_mem) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "Memory for staging buffer was not succesfully allocated");
    }
    vkBindBufferMemory(vulkan_backend->get_device(), stage_buf, stage_mem, 0);

    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(vulkan_backend->get_device(), &alloc_info, nullptr, &index_mem) != VK_SUCCESS) {
        throw harpy_little_error(error_severity::wrong_init, "Memory for vertex buffer was not succesfully allocated");
    }
    vkBindBufferMemory(vulkan_backend->get_device(), index_buf, index_mem, 0);

    void* data;
    vkMapMemory(vulkan_backend->get_device(), stage_mem, 0, create_info.size, 0, &data);
    std::memcpy(data, indices.data(), create_info.size);
    vkUnmapMemory(vulkan_backend->get_device(), stage_mem);

    utilities::copy_vk_buffer(stage_buf, index_buf, create_info.size,
                              vulkan_backend->get_pools().get_first_copy_pool(), vulkan_backend->get_device(), vulkan_backend->get_graph_queue());

    vkDestroyBuffer(vulkan_backend->get_device(), stage_buf, nullptr);
    vkFreeMemory(vulkan_backend->get_device(), stage_mem, nullptr);
}

uint32_t harpy_nest::index_buffer::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) const
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(vulkan_backend->get_ph_device(), &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) 
            return i;
    }

    throw harpy_little_error("failed to find suitable memory type!");
}

void harpy_nest::index_buffer::init_buffer(std::vector<uint32_t> indices)
{
    this->indices = std::move(indices);
    init_index_buffer();
}

void harpy_nest::index_buffer::init_standard_buffer()
{
    init_index_buffer();
}

VkBuffer& harpy_nest::index_buffer::get_vk_buffer()
{
    return index_buf;
}

uint32_t harpy_nest::index_buffer::get_indices_size() const
{
    return indices.size();
}

harpy_nest::index_buffer::~index_buffer()
{
    vkDestroyBuffer(vulkan_backend->get_device(), index_buf, nullptr);
    vkFreeMemory(vulkan_backend->get_device(), index_mem, nullptr);
}
