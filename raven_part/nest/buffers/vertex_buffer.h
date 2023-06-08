﻿#pragma once
#ifndef HARPY_BUFFERS_VERTEX
#define HARPY_BUFFERS_VERTEX
#include <buffers/staging_buffer.h>

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
     
    create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
     
    staging_buffer stage_buf{pool, vulkan_backend};
    stage_buf.init(buffer_size);
    stage_buf.copy_into_buffer(*this);
   }

   operator VkBuffer&() override {
    return buffer;
   }

   std::vector<vertex> get_vertices(){return vertices;}
   

    

  };
 }
#endif //HARPY_BUFFERS_VERTEX