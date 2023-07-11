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
#include <ECS/Entity.h>
#include <vector>
#include <memory>

namespace harpy::nest
{

	struct renderer_context;

	struct render_shared_resources{
		bool updated;
		VkCommandPool command_pool;

		renderer_context* r_context;

		std::vector<harpy::human_part::ECS::Entity*> queue;
		VkCommandBuffer cmd = nullptr;

		//color buffer
		VkDeviceMemory color_image_memory;
		VkImage color_image = nullptr;
		VkImageView color_image_view = nullptr;

		//depth x stencil buffer
		VkDeviceMemory depth_and_stencil_image_memory;
		VkImage depth_and_stencil_image = nullptr;
		VkImageView depth_and_stencil_image_view = nullptr;


		VkFramebuffer fb = nullptr;


		VkSemaphore sem; //for signalising that render finished or blending finished
		VkSemaphore sem2; //rsr used in blending (blocks next frame when post rendering-RSRs on blending)

		void init_images();
		void init_sem();
		void init_command_buffer();
		//void init_framebuffer();


	public:
		render_shared_resources(renderer_context* r_context, VkCommandPool command_pool){
			updated = false;
			this->command_pool = command_pool;
			this->r_context = r_context;
			init_command_buffer();
			init_sem();
			init_images();
			//init_framebuffer();

		}

		VkCommandBuffer getCmd() const {
				return cmd;
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
