/*
 * renderer_context.h
 *
 *  Created on: Jul 8, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERER_CONTEXT_H_
#define RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERER_CONTEXT_H_

#include <spinal_cord/vulkan_spinal_cord.h>
#include "RendererResourceStorage.h"
#include "RendererObjectMapper.h"
#include <swapchain/swapchain.h>
#include <renderer_context/render_shared_resources.h>
#include <raven_part.h>
#include <ECS/Entity.h>

#include <vector>
#include <memory>
#include <atomic>
#include <queue>

using namespace harpy::nest;
using namespace harpy::raven_part;
namespace harpy::nest
{

	//struct swapchain;

	struct render_shared_resources;
	struct vulkan_spinal_cord;
	struct base_window_layout;
	struct object_source;

	struct renderer_context{


		struct RSRPool{
			std::mutex lock;
			std::queue<size_t> queue;
			std::vector<nest::render_shared_resources> RSRs;

		public:
			RSRPool(renderer_context* r_context);
			std::pair<nest::render_shared_resources,size_t> grab();
			void acquire(size_t index);
			~RSRPool(){

				lock.lock();
				if (queue.size() != RSRs.size()) throw harpy::utilities::harpy_little_error("Trying to destruct RSRPool with RSRs, which in use");

				delete &queue;
				for (int i = 0; i < RSRs.size(); ++i) {
					delete &RSRs[i];
				}
				delete &RSRs;
				delete &lock;
			}


		};

		std::shared_ptr<nest::vulkan_spinal_cord> spinal_cord;

		struct swapchain swapchain;
		std::unique_ptr<base_window_layout> connected_window_layout;


		VkRenderPass render_pass;


		VkDescriptorSetLayout descriptor_set_layout = nullptr;
		VkPipelineLayout pipeline_layout = nullptr;
		struct RSRPool rsr_pool{this};
		VkDescriptorPool desc_pool = nullptr;

		nest::RendererResourceStorage storage;
		nest::RendererObjectMapper mapper;

		void render_task(std::queue<harpy::human_part::ECS::Entity*>* queue,
				nest::render_shared_resources rsr,
				VkQueue * vk_queue);

		void init_swapchain();
		void init_render_pass();
		void init_layouts();
		void init_rsr_pool();
		void init_descriptor_pool();
		void init_renderer_resource_storage();
		void init_renderer_object_mapper();

	public:
		renderer_context(std::shared_ptr<vulkan_spinal_cord> cord, std::unique_ptr<base_window_layout> connected_window_layout);
		void render_loop(harpy::raven_part::object_source& source, std::atomic_bool cond);
		VkPipelineLayout getPipelineLayout() const {
					return pipeline_layout;
				}

		VkRenderPass getRenderPass() const {
					return render_pass;
				}

		const struct RSRPool& getRsrPool() const {
					return rsr_pool;
				}

		const std::shared_ptr<vulkan_spinal_cord> getSpinalCord() const {
					return spinal_cord;
				}
		~renderer_context();
	};
}




#endif /* RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERER_CONTEXT_H_ */