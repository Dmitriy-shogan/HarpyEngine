/*
 * harpy.h
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */

#ifndef HARPY_H_
#define HARPY_H_

#include <raven_part.h>
#include <human_part.h>
#include <memory>
#include <future>

namespace harpy{
	extern std::unique_ptr<harpy::nest::renderer_context> r_context_ptr;
	const std::chrono::milliseconds sleepDuration(1);
	const std::vector<uint16_t> indices = {
		    0, 1, 2, 2, 3, 0
		};

	std::pair<VkBuffer,VkDeviceMemory> createVertexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
	std::pair<VkBuffer,VkDeviceMemory> createIndexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
	std::pair<VkPipeline,VkPipelineLayout> createGraphicsPipeline(std::shared_ptr<vulkan_spinal_cord> cord,VkDescriptorSetLayout descriptorSetLayout, VkRenderPass renderpass);
	VkDescriptorSetLayout createDescriptorSetLayout(std::shared_ptr<vulkan_spinal_cord> cord);
	VkDescriptorPool createDescriptorPool(std::shared_ptr<vulkan_spinal_cord> cord);
	std::vector<VkDescriptorSet> createDescriptorSets(std::shared_ptr<vulkan_spinal_cord> cord, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, VkBuffer uniformBuffer);
	void updateUniformBuffer(VkExtent2D extent, std::vector<void*> uniformBuffersMapped);
	std::pair<std::vector<VkBuffer>, std::pair<std::vector<VkDeviceMemory>, std::vector<void*>>> createUniformBuffers(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_pool, VkQueue copy_queue);

	void render(std::unique_ptr<harpy::nest::renderer_context> r_context_ptr, std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr);
	void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::vector<human_part::ECS::Entity*> entities);

	static void init(){
		harpy::nest::vulkan_spinal_cord::init();
	}
	static void run(){
		std::unique_ptr<harpy::nest::base_window_layout> layout = std::make_unique<harpy::nest::base_window_layout>();



		r_context_ptr = std::make_unique<harpy::nest::renderer_context>(harpy::nest::vulkan_spinal_cord::getInstance(), std::move(layout));

		r_context_ptr->init();

		raven_part::resource_types::View view{};
		view.viewport.x = 0.0f;
		view.viewport.y = 0.0f;
		view.viewport.width = static_cast<float>(r_context_ptr->swapchain.extent.width);
		view.viewport.height = static_cast<float>(r_context_ptr->swapchain.extent.height);
		view.viewport.minDepth = 0.0f;
		view.viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		view.scissor.offset = {0, 0};
		view.scissor.extent = r_context_ptr->swapchain.extent;

		std::shared_ptr<vulkan_spinal_cord> cord =  vulkan_spinal_cord::getInstance();

		std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> transfer_queue = cord->queue_supervisor.lock_grab(VK_QUEUE_TRANSFER_BIT);



		std::pair<std::vector<VkBuffer>, std::pair<std::vector<VkDeviceMemory>, std::vector<void*>>> ubo = createUniformBuffers(cord, transfer_queue.first.second, transfer_queue.first.first);
		updateUniformBuffer(r_context_ptr->swapchain.extent, ubo.second.second);


		raven_part::resource_types::Shape shape{};
		shape.vertexBuffer = createVertexBuffer(cord, transfer_queue.first.second, transfer_queue.first.first).first;

		shape.indices_size = indices.size();
		shape.indexBuffer = createIndexBuffer(cord, transfer_queue.first.second, transfer_queue.first.first).first;
		shape.indexType = VK_INDEX_TYPE_UINT16;

		VkDescriptorSetLayout descriptor_set_layout = createDescriptorSetLayout(cord);
		VkDescriptorPool pool = createDescriptorPool(cord);
		std::vector<VkDescriptorSet> sets = createDescriptorSets(cord, pool, descriptor_set_layout, ubo.first[0]);

		raven_part::resource_types::Material material{};
		std::pair<VkPipeline,VkPipelineLayout> pipe = createGraphicsPipeline(cord, descriptor_set_layout, r_context_ptr->render_pass);
		material.pipeline = pipe.first;
		material.pipelineLayout = pipe.second;
		material.desc_set = sets[0];

		renderer_mappings mappings{};
		uint32_t view_id = r_context_ptr->register_view(view);
		mappings.shape_id = r_context_ptr->register_shape(shape);
		mappings.material_id = r_context_ptr->register_material(material);

		human_part::ECS::Entity* entity1 = new human_part::ECS::Entity();
		human_part::ECS::Renderer* rend = new human_part::ECS::Renderer();
		entity1->add_component(rend);

		r_context_ptr->register_renderer(rend, mappings);

		std::vector<human_part::ECS::Entity*> entities{entity1};
		std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr = std::make_shared<harpy::raven_part::scene_source>();
		obj_str_ptr->view_id = view_id;
		obj_str_ptr->consumed.test_and_set();
		std::cout<<"еще нет"<<std::endl;
//		std::async(std::launch::async, [obj_str_ptr,entities]() {
//			physics(obj_str_ptr,entities);
//		    });
		std::thread t(physics,obj_str_ptr,entities);
		std::cout<<"уже да"<<std::endl;
		std::thread t1(render, std::move(r_context_ptr), obj_str_ptr);
		t1.join();
		std::cout<<"дычапщха"<<std::endl;

	}



}

#endif /* HARPY_H_ */
