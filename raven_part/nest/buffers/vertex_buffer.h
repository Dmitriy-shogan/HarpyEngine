#pragma once
#ifndef HARPY_BUFFERS_VERTEX
#define HARPY_BUFFERS_VERTEX
#include <buffers/base_buffer.h>


namespace harpy::nest::buffers {

  class vertex_buffer : public base_buffer
  {
   std::vector<vertex> vertices{
      {{-0.5f, -0.5f,},{1.0f, 0.0f, 0.0f}},
      {{0.5f, -0.5f},{0.0f, 1.0f, 0.0f}},
      {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
      {{-0.5f, 0.5f},{1.0f, 1.0f, 1.0f}}
   };;
   
  public:
   vertex_buffer(pools::command_pool& pool, vulkan_spinal_cord& cord)
            : base_buffer(pool, cord) {}

   

   void set_vertices(std::vector<vertex> vertices)
   {
    this->vertices = std::move(vertices);
   }

   void init() override
   {
    buffer_size = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    create_buffer(vulkan_backend.get_vk_device(), vulkan_backend.get_vk_physical_device(), buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vulkan_backend.get_vk_device(), stagingBufferMemory, 0, buffer_size, 0, &data);
    memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
    vkUnmapMemory(vulkan_backend.get_vk_device(), stagingBufferMemory);
    
    create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    utilities::vk_copy_buffer(stagingBuffer, buffer, buffer_size, vulkan_backend.get_vk_device(), pool, vulkan_backend.get_vk_graphics_queue());

    vkDestroyBuffer(vulkan_backend.get_vk_device(), stagingBuffer, nullptr);
    vkFreeMemory(vulkan_backend.get_vk_device(), stagingBufferMemory, nullptr);
   }

   operator VkBuffer&() override {
    return buffer;
   }

   std::vector<vertex> get_vertices(){return vertices;}
   

    

  };
 }
#endif //HARPY_BUFFERS_VERTEX