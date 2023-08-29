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

	const std::string HARPY_DATAGUIDE_URI = "harpy.dataguide";

	struct dataguide{
		std::string gltf_uri;

	};

	struct harpy_state{
		uint32_t scene_id;

	};



	void render(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* cond);
	void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::vector<human_part::ECS::Entity*> entities, std::atomic_flag* cond);




	struct scene_workaround_unit{
		std::thread physics_thread;
		std::thread render_thread;
		std::atomic_flag rend_cond{true};
		std::atomic_flag phys_cond{true};

		std::shared_ptr<harpy::raven_part::scene_source> scene_source{};

		std::shared_ptr<renderer_context> r_context_ptr;

		scene_workaround_unit(std::shared_ptr<renderer_context> r_context_ptr){
			this->r_context_ptr = r_context_ptr;
		}

		void load(tinygltf::Model model, uint32_t scene_id){
			scene_source->load_scene(r_context_ptr, model, scene_id);
		}

		void start(){
			rend_cond.test_and_set();
			phys_cond.test_and_set();

			physics_thread = std::thread(physics,scene_source,&phys_cond);
			render_thread = std::thread(render, r_context_ptr, scene_source, &rend_cond);

		}

		void stop(){
			rend_cond.clear();
			phys_cond.clear();

			physics_thread.join();
			render_thread.join();
		}

	};



	extern std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;
	const std::chrono::milliseconds sleepDuration(100);

	static struct dataguide dataguide{};
	static struct harpy_state state{};
	static tinygltf::Model model;

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

	void load_dataguide(){

		toml::table tbl;
		try
		{
			tbl = toml::parse_file(HARPY_DATAGUIDE_URI);
			dataguide.gltf_uri = *tbl["gltf"]["uri"].value<std::string>();
		}
		catch (const toml::parse_error& err)
		{
			throw harpy::utilities::harpy_little_error("Failed to load dataguide");
		}

	}

	void load_gltf(){

	    tinygltf::TinyGLTF loader;
	    std::string err;
	    std::string warn;

	    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, dataguide.gltf_uri);
	    if (!warn.empty()) {
	        std::cout << "WARN: " << warn << std::endl;
	    }

	    if (!err.empty()) {
	    	throw harpy::utilities::harpy_little_error("Failed to load gltf scene");
	    }

	}

	static void init(){
		load_dataguide();
		load_gltf();
		harpy::nest::vulkan_spinal_cord::init();

	}
	static void run(){


		//std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr = std::make_shared<harpy::raven_part::scene_source>();

		std::unique_ptr<harpy::nest::base_window_layout> layout = std::make_unique<harpy::nest::base_window_layout>();



		r_context_ptr = std::make_unique<harpy::nest::renderer_context>(harpy::nest::vulkan_spinal_cord::getInstance(), std::move(layout));

		r_context_ptr->init();

		struct scene_workaround_unit scene{r_context_ptr};

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
		uint32_t view_id = obj_str_ptr->storage.register_view(view);
		mappings.shape_id = obj_str_ptr->storage.register_shape(shape);


		mappings.material_id = obj_str_ptr->storage.register_material(material);
		std::cout<<"mappings.shape_id"<<std::endl;
		std::cout<<mappings.shape_id<<std::endl;
		renderer_mappings mappings2{};
		mappings2.material_id = mappings.material_id;
		mappings2.shape_id = obj_str_ptr->storage.register_shape(shape2);
		std::cout<<mappings2.shape_id<<std::endl;


		human_part::ECS::Entity* entity1 = new human_part::ECS::Entity();
		human_part::ECS::Renderer* rend = new human_part::ECS::Renderer();
		human_part::ECS::Transform* tr1 = new human_part::ECS::Transform();
		entity1->add_component(rend);
		entity1->add_component(tr1);


		obj_str_ptr->mapper.register_renderer(rend, mappings);

		human_part::ECS::Entity* entity2 = new human_part::ECS::Entity();
		human_part::ECS::Renderer* rend2 = new human_part::ECS::Renderer();
		human_part::ECS::Transform* tr2 = new human_part::ECS::Transform();
		entity2->add_component(rend2);
		entity2->add_component(tr2);
		obj_str_ptr->mapper.register_renderer(rend2, mappings2);

		std::cout<<"obj_str_ptr->mapper.register_renderer(rend2, mappings2)"<<std::endl;

		human_part::ECS::Entity* cam = new human_part::ECS::Entity();
		human_part::ECS::Transform* tr3 = new human_part::ECS::Transform(glm::vec3{0.0f,0.0f,-2.0f});
		human_part::ECS::Camera* cm = new human_part::ECS::Camera();
		cm->view_id = obj_str_ptr->storage.register_view(view);
		cam->add_component(tr3);
		cam->add_component(cm);

		//std::vector<human_part::ECS::Entity*> entities{entity1,entity2};

		obj_str_ptr->camera = cam;
		obj_str_ptr->consumed.test_and_set();



		start workaround HERE;

		std::cout<<"дычапщха"<<std::endl;

	}



}

#endif /* HARPY_H_ */
