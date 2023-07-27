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
#include <bias/vertex.h>
#include <memory>
#include <future>

namespace harpy{
	extern std::unique_ptr<harpy::nest::renderer_context> r_context_ptr;
	const std::chrono::milliseconds sleepDuration(100);
	const std::vector<uint16_t> indices = {
		    0, 1, 2, 2, 3, 0
		};

	const std::vector<Vertex> vertices = {
	    {{-0.9f, 0.1717f,0.0f}, {1.0f, 0.0f, 0.0f}},
	    {{0.9f, 0.1717f,0.0f}, {0.0f, 1.0f, 0.0f}},
	    {{0.9f, -0.1717f,0.0f}, {0.0f, 0.0f, 1.0f}},
	    {{-0.9f, -0.1717f,0.0f}, {1.0f, 1.0f, 1.0f}}
	};

	const std::vector<Vertex> vertices2 = {
		    {{0.0f, 0.0f,1.0f}, {0.0f, 0.0f, 0.5f}},
		    {{0.5f, 0.0f,1.0f}, {0.0f, 0.0f, 0.5f}},
		    {{0.5f, 0.5f,1.0f}, {0.0f, 0.0f, 0.5f}},
		    {{0.0f, 0.5f,1.0f}, {0.0f, 0.0f, 0.5f}}
		};

	std::pair<VkBuffer,VkDeviceMemory> createVertexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
	std::pair<VkBuffer,VkDeviceMemory> createIndexBuffer(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
	std::pair<VkBuffer,VkDeviceMemory> createVertexBuffer2(std::shared_ptr<vulkan_spinal_cord> cord, VkCommandBuffer copy_buf, VkQueue copy_queue);
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
		view.view_field = glm::vec2{1.0f,1.0f};
		view.cameraType = raven_part::resource_types::View::CameraType::ORTHOGRAPHIC;
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
		shape.vert_size = vertices.size();
		shape.indices_size = indices.size();
		shape.indexBuffer = createIndexBuffer(cord, transfer_queue.first.second, transfer_queue.first.first).first;
		shape.indexType = VK_INDEX_TYPE_UINT16;


		raven_part::resource_types::Shape shape2{};
		shape2.vertexBuffer = createVertexBuffer2(cord, transfer_queue.first.second, transfer_queue.first.first).first;
		shape2.vert_size = vertices2.size();
		shape2.indices_size = indices.size();
		shape2.indexBuffer = createIndexBuffer(cord, transfer_queue.first.second, transfer_queue.first.first).first;
		shape2.indexType = VK_INDEX_TYPE_UINT16;

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
		std::cout<<"mappings.shape_id"<<std::endl;
		std::cout<<mappings.shape_id<<std::endl;
		renderer_mappings mappings2{};
		mappings2.material_id = mappings.material_id;
		mappings2.shape_id = r_context_ptr->register_shape(shape2);
		std::cout<<mappings2.shape_id<<std::endl;


		human_part::ECS::Entity* entity1 = new human_part::ECS::Entity();
		human_part::ECS::Renderer* rend = new human_part::ECS::Renderer();
		human_part::ECS::Transform* tr1 = new human_part::ECS::Transform();
		entity1->add_component(rend);
		entity1->add_component(tr1);
		r_context_ptr->register_renderer(rend, mappings);

		human_part::ECS::Entity* entity2 = new human_part::ECS::Entity();
		human_part::ECS::Renderer* rend2 = new human_part::ECS::Renderer();
		human_part::ECS::Transform* tr2 = new human_part::ECS::Transform();
		entity2->add_component(rend2);
		entity2->add_component(tr2);
		r_context_ptr->register_renderer(rend2, mappings2);



		human_part::ECS::Entity* cam = new human_part::ECS::Entity();
		human_part::ECS::Transform* tr3 = new human_part::ECS::Transform(glm::vec3{0.0f,0.0f,-2.0f});
		human_part::ECS::Camera* cm = new human_part::ECS::Camera();
		cm->view_id = r_context_ptr->register_view(view);
		cam->add_component(tr3);
		cam->add_component(cm);

		std::vector<human_part::ECS::Entity*> entities{entity1,entity2};
		std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr = std::make_shared<harpy::raven_part::scene_source>();
		obj_str_ptr->camera = cam;
		obj_str_ptr->consumed.test_and_set();
//		std::async(std::launch::async, [obj_str_ptr,entities]() {
//			physics(obj_str_ptr,entities);
//		    });
		std::thread t(physics,obj_str_ptr,entities);
		//std::cout<<"уже да"<<std::endl;
		std::thread t1(render, std::move(r_context_ptr), obj_str_ptr);
		t1.join();
		std::cout<<"дычапщха"<<std::endl;

	}



}

#endif /* HARPY_H_ */
