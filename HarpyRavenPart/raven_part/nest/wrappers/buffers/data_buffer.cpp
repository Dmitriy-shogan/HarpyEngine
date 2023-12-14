#include "data_buffer.h"

void harpy::nest::wrappers::data_buffer::init(uint32_t size, buffer_type type, VkDevice* device, VmaAllocator* allocator)
{
    this->allocator = allocator;
    this->device = device;
    this->type = type;

    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    
    if(type == buffer_type::indice)
    {
        ci.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        this->size = size * sizeof(uint32_t);
    } else
    {
        ci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        this->size = size * sizeof(vertex);
    }
    ci.size = this->size;
    //Just for now, needs research
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.pNext = nullptr;

    VmaAllocationCreateInfo alloc_ci{};
    alloc_ci.usage = VMA_MEMORY_USAGE_AUTO;

    HARPY_VK_CHECK(vmaCreateBuffer(*allocator, &ci, &alloc_ci, &buffer, &alloc, nullptr));
}

harpy::nest::wrappers::data_buffer::data_buffer(data_buffer& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
}

harpy::nest::wrappers::data_buffer::data_buffer(const data_buffer& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
}

harpy::nest::wrappers::data_buffer harpy::nest::wrappers::data_buffer::operator=(data_buffer& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
    return *this;
}

harpy::nest::wrappers::data_buffer harpy::nest::wrappers::data_buffer::operator=(const data_buffer& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
    return *this;
}

harpy::nest::wrappers::data_buffer::data_buffer(data_buffer&& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
}

harpy::nest::wrappers::data_buffer harpy::nest::wrappers::data_buffer::operator=(data_buffer&& buffer)
{
    size = buffer.size;
    device = buffer.device;
    allocator = buffer.allocator;
    return *this;
}

VkBuffer& harpy::nest::wrappers::data_buffer::get_vk_buffer()
{return buffer;
}

harpy::nest::wrappers::data_buffer::operator VkBuffer_T*&()
{return buffer;
}

VmaAllocation& harpy::nest::wrappers::data_buffer::get_vma_allocation()
{return alloc;
}

harpy::nest::wrappers::data_buffer::operator VmaAllocation_T*&()
{return alloc;
}

uint32_t harpy::nest::wrappers::data_buffer::get_size()
{return size;
}

harpy::nest::wrappers::buffer_type harpy::nest::wrappers::data_buffer::get_type()
{return type;
}

harpy::nest::wrappers::data_buffer::operator buffer_type()
{return type;
}

harpy::nest::wrappers::data_buffer::~data_buffer()
{
    if(buffer)
        vmaDestroyBuffer(*allocator, buffer, alloc);
}
