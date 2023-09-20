/*
 * render_shared_resources.h
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_NEST_RENDERER_CONTEXT_RENDER_SHARED_RESOURCES_H_
#define RAVEN_PART_NEST_RENDERER_CONTEXT_RENDER_SHARED_RESOURCES_H_

#include <spinal_cord/vulkan_spinal_cord.h>
#include <renderer_context/RendererObjectMapper.h>
#include <ECS/ECS.h>
#include <vector>
#include <memory>

namespace harpy::nest
{

	struct renderer_context;

	struct render_shared_resources{
		bool updated;

		renderer_context* r_context;

		std::vector<std::pair<harpy::human_part::ECS::Transform*, std::vector<renderer_mappings>>> queue{};

		//color buffer
		VkDeviceMemory color_image_memory = nullptr;
		VkImage color_image = nullptr;
		VkImageLayout color_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageView color_image_view = nullptr;

		//depth x stencil buffer
		VkDeviceMemory depth_and_stencil_image_memory = nullptr;
		VkImage depth_and_stencil_image = nullptr;
		VkImageLayout depth_and_stencil_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkImageView depth_and_stencil_image_view = nullptr;
		VkImageView depth_image_view = nullptr;
		VkImageView stencil_image_view = nullptr;

		VkFramebuffer fb = nullptr;


		VkDescriptorSet blender_set1 = nullptr;
		VkDescriptorSet blender_set2 = nullptr;
		VkDescriptorSet blender_set_out = nullptr;
		VkDescriptorPool blender_desc_pool = nullptr;
		VkSemaphore sem = nullptr; //for signalising that render finished or blending finished
		VkFence fence1 = nullptr;
		VkFence blender_fence = nullptr;

		bool wait_needed = false;


		VkDescriptorSet vert_desc = nullptr;


		void init_images();
		void init_sem();
		void reinit_vertex_tmp(VkDescriptorPool vert_desc_pool, VkDescriptorSetLayout layout);
		void init_blender_pool();
		void wait();
		void reset();

	public:
		render_shared_resources(renderer_context* r_context){
			updated = false;
			this->r_context = r_context;
		}

		void init(){
			init_sem();
			init_images();
			init_blender_pool();

		}

		VkFramebuffer getFb() const {
				return fb;
			}

		bool isUpdated() const {
				return updated;
			}

		void setUpdated(bool updated) {
				this->updated = updated;
			}
		~render_shared_resources();

	};
}

#endif /* RAVEN_PART_NEST_RENDERER_CONTEXT_RENDER_SHARED_RESOURCES_H_ */
