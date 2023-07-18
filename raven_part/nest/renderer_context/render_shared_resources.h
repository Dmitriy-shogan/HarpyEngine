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
#include <ECS/Entity.h>
#include <vector>
#include <memory>

namespace harpy::nest
{

	struct renderer_context;

	struct render_shared_resources{
		bool updated;


		renderer_context* r_context;

		std::vector<std::pair<harpy::human_part::ECS::Entity*, renderer_mappings>> queue{};

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

		VkDescriptorPool blender_desc_pool = nullptr;
		VkDescriptorSet blender_set1 = nullptr;
		VkDescriptorSet blender_set2 = nullptr;
		VkDescriptorSet blender_set_out = nullptr;


		VkSemaphore sem = nullptr; //for signalising that render finished or blending finished
		//VkSemaphore sem2 = nullptr; //rsr used in blending (blocks next frame when post rendering-RSRs on blending)
		VkFence fence1 = nullptr;
		VkFence blender_fence = nullptr;

		bool wait_needed = false;

		void init_images();
		void init_sem();
		void init_blender_pool();
		void wait();
		void reset();
		//void init_framebuffer();


	public:
		render_shared_resources(renderer_context* r_context){
			std::cout<<"render_shared_resources RSR"<<std::endl;
			updated = false;
			this->r_context = r_context;
			//init_framebuffer();

		}

		void init(){
			std::cout<<"init RSR"<<std::endl;
			init_sem();
			std::cout<<"init_sem succ"<<std::endl;
			init_images();
			init_blender_pool();
			std::cout<<"init_images succ"<<std::endl;
			std::cout<<"init RSR succ"<<std::endl;
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
