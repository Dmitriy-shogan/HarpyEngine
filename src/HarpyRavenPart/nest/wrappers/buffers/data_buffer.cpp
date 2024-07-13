#include <nest/wrappers/buffers/data_buffer.h>

#include <nest/pipeline/descriptors/uniform_buffers_descriptions.h>

void harpy::nest::wrappers::data_buffer::init(std::size_t size)
{
    if(buffer)
        vmaDestroyBuffer(*allocator, buffer, alloc);
    
    VkBufferCreateInfo ci{};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    switch(type)
    {
    case buffer_type::vertex:
        {
            ci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            this->size = size * sizeof(vertex);
            break;
        }
    case buffer_type::indice:
        {
            ci.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
            this->size = size * sizeof(uint32_t);
            break;
        }
    case buffer_type::staging:
        {
            ci.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            this->size = size;
            break;
        }
    case buffer_type::uniform:
        {
            ci.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            this->size = size;
            break;
        }
    default:
        throw utilities::error_handling::harpy_little_error("What a joker, decided to create a buffer with an enum that is not even bm!");
    }
    ci.size = this->size;
    //Just for now, needs research
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ci.pNext = nullptr;

    VmaAllocationCreateInfo alloc_ci{};
    if(type == buffer_type::staging )
    {
        alloc_ci.usage =  VMA_MEMORY_USAGE_CPU_TO_GPU;
        alloc_ci.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

    } else if(type == buffer_type::uniform)
    {
        alloc_ci.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    } else
    {
        alloc_ci.usage = VMA_MEMORY_USAGE_AUTO;
    }

    HARPY_VK_CHECK(vmaCreateBuffer(*allocator, &ci, &alloc_ci, &buffer, &alloc, nullptr));
}

harpy::nest::wrappers::data_buffer::data_buffer(buffer_type type, std::size_t size, VkDevice* device,
                                                VmaAllocator* allocator) : allocator(allocator), device(device), type(type)
{
    if (size == 0) return;
    init(size);
}

harpy::nest::wrappers::data_buffer::data_buffer(data_buffer&& buffer) noexcept
{
    size = buffer.size;
    type = buffer.type;
    device = buffer.device;
    allocator = buffer.allocator;
    alloc = buffer.alloc;
    buffer.alloc = nullptr;
    this->buffer = buffer.buffer;
    buffer.buffer = nullptr;
}

harpy::nest::wrappers::data_buffer& harpy::nest::wrappers::data_buffer::operator=(data_buffer&& buffer) noexcept
{
    size = buffer.size;
    type = buffer.type;
    device = buffer.device;
    allocator = buffer.allocator;
    alloc = buffer.alloc;
    buffer.alloc = nullptr;
    this->buffer = buffer.buffer;
    buffer.buffer = nullptr;
    return *this;
}

VkBuffer& harpy::nest::wrappers::data_buffer::get_vk_buffer()
{return buffer;
}

harpy::nest::wrappers::data_buffer::operator VkBuffer_T*&()
{return buffer;
}

const VmaAllocation&  harpy::nest::wrappers::data_buffer::get_vma_allocation()
{return alloc;
}

harpy::nest::wrappers::data_buffer::operator VmaAllocation_T*&()
{return alloc;
}

const uint32_t harpy::nest::wrappers::data_buffer::get_size()
{return size;
}

harpy::nest::wrappers::buffer_type const harpy::nest::wrappers::data_buffer::get_type(){return type;}

harpy::nest::wrappers::data_buffer::operator  harpy::nest::wrappers::buffer_type const() {return type;}

VkDevice*& harpy::nest::wrappers::data_buffer::get_vk_device()
{return device;}

VmaAllocator*& harpy::nest::wrappers::data_buffer::get_vk_allocator()
{return allocator;
}

void* harpy::nest::wrappers::data_buffer::get_mapped_ptr()
{
    void* ptr{nullptr};
    vmaMapMemory(*allocator, alloc, &ptr);
    return ptr;
}

void harpy::nest::wrappers::data_buffer::unmap_ptr()
{
    vmaUnmapMemory(*allocator, alloc);
}

harpy::nest::wrappers::data_buffer::~data_buffer()
{
    if(buffer)
        vmaDestroyBuffer(*allocator, buffer, alloc);
}
