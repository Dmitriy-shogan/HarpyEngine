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

	struct load_package{
		std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;
		VkCommandBuffer copy_buf;
		VkQueue copy_queue;

		load_package(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, VkQueue copy_queue, VkCommandBuffer copy_buf){
			this->copy_buf = copy_buf;
			this->copy_queue = copy_queue;
			this->r_context_ptr = r_context_ptr;
		}
	};

	struct preload_map{
		std::map<uint32_t, uint32_t> material_map;
		std::map<std::map<std::string, int>, uint32_t> shape_map;

		bool isMaterialLoaded(uint32_t gltf_material_id){
			return material_map.count(gltf_material_id) > 0;
		}

		bool isShapeLoaded(uint32_t gltf_indice_id, std::map<std::string, int> gltf_shape_attribs){
			gltf_shape_attribs["__INDICES"] = gltf_indice_id;
			return shape_map.count(gltf_shape_attribs) > 0;
		}

		uint32_t getMaterialId(uint32_t gltf_material_id){
			return material_map[gltf_material_id];
		}

		uint32_t getShapeId(int32_t gltf_indice_id, std::map<std::string, int> gltf_shape_attribs){
			gltf_shape_attribs["__INDICES"] = gltf_indice_id;
			return shape_map[gltf_shape_attribs];
		}

		void setMaterialId(uint32_t gltf_material_id, uint32_t id){
			material_map[gltf_material_id] = id;
		}

		void setShapeId(int32_t gltf_indice_id, std::map<std::string, int> gltf_shape_attribs, uint32_t id){
			gltf_shape_attribs["__INDICES"] = gltf_indice_id;
			shape_map[gltf_shape_attribs] = id;
		}


	};

	struct scene_source{
		std::mutex lock;
		std::atomic_flag consumed;
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities{};

		struct nest::RendererResourceStorage storage{};
		nest::RendererObjectMapper mapper{};

		uint32_t curr_camera = 0;
		std::vector<human_part::ECS::Entity*> cameras{};

		void set_active_camera(uint32_t id){
			curr_camera = id;
		}
		uint32_t add_camera(human_part::ECS::Entity* camera){
			this->cameras.push_back(camera);
			return cameras.size()-1;
		};

		void remove_camera(human_part::ECS::Entity* camera){
			for (auto it = cameras.begin(); it != cameras.end(); it++) {
				cameras.erase(it);
				return;
			}
		};

		void remove_camera(uint32_t id){
					cameras.erase(cameras.begin() + id);
				};

		scene_source(){
			cameras.resize(0);

			entities = std::make_shared<std::vector<human_part::ECS::Entity*>>();
		}

//		void r_init(tinygltf::Model& model, tinygltf::Primitive& prim, load_package pack){
//			storage.r_init(model, prim, pack);
//		};


		void load_scene(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, tinygltf::Model model, uint32_t scene_id);

		uint32_t create_entity(){


			entities->push_back(new harpy::human_part::ECS::Entity());

			return entities->size() - 1;
		}

		uint32_t create_entity(uint32_t parent_id){
					entities->push_back(new harpy::human_part::ECS::Entity(parent_id));
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

		void loadNode(tinygltf::Model& model, tinygltf::Node& node, struct preload_map preload_map, struct load_package pack){

			uint32_t entity_id = create_entity();

			if(has_transform(model, node)){
				entity_load_transform_component(entity_id, model, node);
			}else{
				entity_create_transform_component(entity_id);
			}

			if ((node.camera) >= 0 && (node.camera) < model.cameras.size()){

				cameras.push_back((*entities)[entity_id]);
				entity_load_camera_component(entity_id, model, node, pack);
			}

			if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
				//loadMesh(model, );
				entity_load_renderer_component(entity_id, model, node, preload_map, pack); //model.meshes[node.mesh]
			}

			for (size_t i = 0; i < node.children.size(); i++) {
				assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
				loadNode(model, model.nodes[node.children[i]],preload_map, pack);
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

		void entity_create_transform_component(uint32_t entity_id){
					human_part::ECS::Transform* tr = new human_part::ECS::Transform();
					(*entities)[entity_id]->add_component(tr);
				}

		void entity_load_renderer_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node, struct preload_map preload_map, struct load_package pack){
			human_part::ECS::Renderer* rend = new human_part::ECS::Renderer();
			std::vector<renderer_mappings> mappings{};

			uint32_t prim_count = model.meshes[node.mesh].primitives.size();
			mappings.resize(prim_count);

			for (int i = 0; i < prim_count; ++i) {
				tinygltf::Primitive prim = model.meshes[node.mesh].primitives[i];
				if (preload_map.isMaterialLoaded(prim.material)){
					mappings[i].material_id = preload_map.getMaterialId(prim.material);
				}else{
					raven_part::resource_types::Material material{};
					material.load(model, prim, pack);
					mappings[i].material_id = storage.register_material(material);
					preload_map.setMaterialId(prim.material, mappings[i].material_id);
				}

				if (preload_map.isShapeLoaded(prim.indices, prim.attributes)){
					mappings[i].shape_id = preload_map.getShapeId(prim.indices, prim.attributes);
				}else{
					raven_part::resource_types::Shape shape{};
					shape.load(model, prim, pack);
					mappings[i].shape_id = storage.register_shape(shape);
					preload_map.setShapeId(prim.indices, prim.attributes, mappings[i].shape_id);
				}
			}

			mapper.register_renderer(rend, mappings);
			(*entities)[entity_id]->add_component(rend);
		}

		void entity_load_camera_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node, struct load_package pack);


	};
}



#endif /* RAVEN_PART_RAVEN_PART_H_ */
