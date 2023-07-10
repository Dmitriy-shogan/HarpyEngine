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

namespace harpy::nest
{

	struct renderer_context;

	struct render_shared_resources{
		bool updated;
		VkCommandPool * command_pool;

		renderer_context* r_context;


		const VkCommandBuffer cmd = nullptr;
		VkImage image = nullptr;
		VkImageView image_view = nullptr;
		VkFramebuffer fb = nullptr;

		void init_command_buffer();
		void init_framebuffer();


	public:
		render_shared_resources(renderer_context* r_context, VkCommandPool* command_pool){
			updated = false;
			this->command_pool = command_pool;
			this->r_context = r_context;
			init_command_buffer();
			init_framebuffer();

		}

		VkCommandBuffer getCmd() const {
				return cmd;
			}

		VkFramebuffer getFb() const {
				return fb;
			}

		VkImage getImage() const {
				return image;
			}

		VkImageView getImageView() const {
				return image_view;
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
