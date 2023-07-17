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
#include <windowing/base_window_layout.h>
#include <raven_part.h>
#include <ECS/Entity.h>

#include <vector>
#include <memory>
#include <atomic>
#include <queue>
#include <thread>
using namespace harpy::nest;
using namespace harpy::raven_part;

const std::chrono::milliseconds sleepDuration(1);

namespace harpy::raven_part{
	struct object_source;
}

namespace harpy::nest
{

	//struct swapchain;
	const uint32_t effective_rsr_cnt = 32;//std::max((uint32_t)RSR_ABSURD_LIMIT, (uint32_t)queues_cnt, (uint32_t)(DEV_MEM_RENDER_PERSENTAGE * DEV_MEM_SIZE / RSR_GPU_MEM_COST));
	struct render_shared_resources;
	struct vulkan_spinal_cord;
	struct base_window_layout;

	struct renderer_context{



		struct blender_push_constants{
			uint32_t layers_cnt;
		};


		struct RSRPool{
			std::mutex lock;
			std::queue<uint32_t> queue{};
			std::vector<nest::render_shared_resources> RSRs{};
			renderer_context* r_context;


		public:
			RSRPool(renderer_context * rend){
				this->r_context = rend;

				//for (int i = 0; i < rsr_cnt; ++i) {
				//}
			}

			std::pair<nest::render_shared_resources*,uint32_t> lock_grab(){
				//std::lock_guard<std::mutex> _(lock);
				std::pair<nest::render_shared_resources*,uint32_t> res_pair;
				lock.lock();
				while(true){
					if (queue.size() > 0){
						uint32_t k = queue.front();
						queue.pop();
						res_pair = std::make_pair(&RSRs[k],k);
						lock.unlock();
						return res_pair;
					}
					lock.unlock();
					std::this_thread::sleep_for(sleepDuration);
				}
			}

			std::pair<nest::render_shared_resources*,uint32_t> grab(){
				while(true){
					if (queue.size() > 0){
						uint32_t k = queue.front();
						queue.pop();
						return std::make_pair(&RSRs[k],k);
					}
					std::this_thread::sleep_for(sleepDuration);
				}
			}

			void free(size_t index){
				queue.push(index);
			}

			void init(uint32_t rsr_cnt){
				std::cout<<"bbb"<<std::endl;
				RSRs.resize(rsr_cnt, {r_context});
				for (int i = 0; i < rsr_cnt; ++i) {
					RSRs[i].init();
					queue.push(i);
				}
			}

			void lock_free(size_t index){
				std::lock_guard<std::mutex> _(lock);
				queue.push(index);
			}

			~RSRPool(){

				lock.lock();
				if (queue.size() != RSRs.size()) throw harpy::utilities::harpy_little_error("Trying to destruct RSRPool with RSRs, which in use");

				//..delete &queue;
				for (int i = 0; i < RSRs.size(); ++i) {
					delete &RSRs[i];
				}
				//delete &RSRs;
				//delete &lock;
			}


		};

		std::shared_ptr<nest::vulkan_spinal_cord> spinal_cord;

		struct swapchain swapchain{this};
		std::unique_ptr<base_window_layout> connected_window_layout;


		VkRenderPass render_pass = nullptr;
		//VkDescriptorSetLayout descriptor_set_layout = nullptr;
		VkPipelineLayout pipeline_layout = nullptr;
		struct RSRPool rsr_pool{this};
		VkDescriptorPool blender_desc_pool = nullptr;


		VkDescriptorSetLayout blender_descriptor_set_layout1 = nullptr;
		VkDescriptorSetLayout blender_descriptor_set_layout2 = nullptr;
		VkDescriptorSetLayout blender_descriptor_set_layout_out = nullptr;
		//VkRenderPass blender_render_pass = nullptr;
		VkPipeline blender_pipeline = nullptr;
		//VkViewport blender_dynamic_viewport{};
		//VkRect2D blender_dynamic_scissors{};
		VkPipelineLayout blender_pipeline_layout = nullptr;
		VkDescriptorSet blender_set1 = nullptr;
		VkDescriptorSet blender_set2 = nullptr;
		VkDescriptorSet blender_set_out = nullptr;
		VkSampler blender_sampler = nullptr;
		VkClearValue clear_color{{0.0f,0.0f,0.0f,1.0f}};
		VkClearValue clear_depth_stencil = {{1.0f, 0}};

		struct blender_push_constants blender_push_constants{};


		nest::RendererResourceStorage storage;
		nest::RendererObjectMapper mapper;

		std::queue<harpy::human_part::ECS::Entity*> queue;

		void render_task(
				std::pair<render_shared_resources*, uint32_t> rsr,
				std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue
				);
		void blending(
				std::pair<render_shared_resources*, uint32_t> rsr,
				std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
				std::vector<std::pair<render_shared_resources*, uint32_t>>* rendered_rsrs,
				VkImage swapchain_image,
				VkImageView swapchain_image_view,
				VkSemaphore image_sem
				);
		void present(
				std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> vk_queue,
				std::pair<render_shared_resources*, uint32_t> rsr,
				uint32_t image_index);

		void init_swapchain();
		void init_render_pass();
		void init_rsr_pool();
		void init_descriptor_pool();
		void init_blender();
		void init_renderer_resource_storage();
		void init_renderer_object_mapper();

	public:
		renderer_context(
				std::shared_ptr<vulkan_spinal_cord> cord,
				std::unique_ptr<base_window_layout> connected_window_layout
				);
		void render_loop(
				std::shared_ptr<harpy::raven_part::object_source> source,
				std::atomic_flag* cond
				);

		void init();
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
