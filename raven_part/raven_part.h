/*
 * raven_part.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_RAVEN_PART_H_
#define RAVEN_PART_RAVEN_PART_H_

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>


#include <ECS/ECS.h>
#include <spinal_cord/vulkan_spinal_cord.h>
#include <renderer_context/renderer_context.h>

//using namespace harpy::human_part::ECS;

namespace nest{
	struct RendererResourceStorage;
}

namespace harpy::raven_part{

	struct scene_source{
		std::mutex lock;
		std::atomic_flag consumed;
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities;

		struct nest::RendererResourceStorage storage{};
		nest::RendererObjectMapper mapper{};

		human_part::ECS::Entity* camera;
		scene_source() = default;

		void r_init(std::shared_ptr<harpy::nest::renderer_context> r_context){
			storage.r_init(r_context);
		};


		void load_scene(std::shared_ptr<renderer_context> r_context_ptr, tinygltf::Model model, uint32_t scene_id){
			const tinygltf::Scene& scene = model.scenes[model.defaultScene];
			std::map<uint32_t, uint32_t> preload_map;
			#ifdef PRELOAD_STRATEGY == PRELOAD_STRATEGY_FULL
				preload_map = preload_resources_full(model);
			#else
				preload_map = preload_resources_part(model,scene);
			#endif

			for (size_t i = 0; i < scene.nodes.size(); ++i) {
				assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
				loadNode(model, model.nodes[scene.nodes[i]]);
			}

			r_init(r_context_ptr);
		}

		uint32_t create_entity(){
			entities->push_back(new harpy::human_part::ECS::Entity());
			return entities->size() - 1;
		}

		uint32_t create_entity(uint32_t parent_id){
					entities->push_back(new harpy::human_part::ECS::Entity());
					(*entities)[entities->size() - 1]->parent_id = parent_id;
					return entities->size() - 1;
				}

		uint32_t create_entity_lock(){
			uint32_t id = -1;
			lock.lock();
			entities->push_back(new harpy::human_part::ECS::Entity());
			id = entities->size() - 1;
			lock.unlock();
			return id;
		}

		void loadNode(tinygltf::Model& model, tinygltf::Node& node){

			uint32_t entity_id = create_entity();
			if(has_transform(model, node)){
				entity_load_transform_component(entity_id, model, node);
			}
			if ((node.camera) >= 1 && (node.camera) < model.cameras.size()){
				entity_load_camera_component(entity_id, model, node);
			}

			if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
				//loadMesh(model, );
				entity_load_renderer_component(entity_id, model, node); //model.meshes[node.mesh]
			}

			for (size_t i = 0; i < node.children.size(); i++) {
				assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
				loadNode(model, model.nodes[node.children[i]]);
			}
		}

		bool has_transform(tinygltf::Model& model, tinygltf::Node& node){
			//return (node.matrix.size() == 16);
			return (node.translation.size() >= 3) && (node.scale.size() >= 3) && (node.rotation.size() >= 4);

		}


		void entity_load_transform_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node){
			human_part::ECS::Transform* tr = new human_part::ECS::Transform();
			tr->pos = glm::vec3{node.translation[0],node.translation[1],node.translation[2]};
			tr->rot = glm::quat{node.rotation[0],node.rotation[1],node.rotation[2],node.rotation[3]};
			tr->scale = glm::vec3{node.scale[0],node.scale[1],node.scale[2]};
			(*entities)[entity_id]->add_component(tr);
		}

		void entity_load_renderer_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node){
			human_part::ECS::Renderer* rend = new human_part::ECS::Renderer();

			raven_part::resource_types::Shape shape{};
			load_shape(shape, model, node);
			raven_part::resource_types::Material material{};
			load_material(material, model, node);

			renderer_mappings mappings{};
			mappings.material_id = preload_map.materials[] //.register_material(material);
			mappings.shape_id = storage.register_shape(shape);

			mapper.register_renderer(rend, mappings);
			(*entities)[entity_id]->add_component(rend);
		}

		void entity_load_camera_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node){
			human_part::ECS::Camera* cm = new human_part::ECS::Camera();
			tinygltf::Camera* cam = model.cameras[node.camera];

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

			storage.register_view(view);
			(*entities)[entity_id]->add_component(cm);
		}


		void load_shape(resource_types::Shape& shape, tinygltf::Model& model, tinygltf::Node& node){
			shape.indexBuffer = loadIndexBuffer();
			shape.vertexBuffer = loadVertexBuffer();
		}

		void load_material(resource_types::Material& material, tinygltf::Model& model, tinygltf::Node& node){
			aaa
		}

		VkBuffer loadIndexBuffer(tinygltf::Model& model, tinygltf::Node& node){

			model.meshes[node.mesh].primitives[]
		}

		void loadMesh(tinygltf::Model& model, tinygltf::Mesh& node){

		}


	};
}



#endif /* RAVEN_PART_RAVEN_PART_H_ */
