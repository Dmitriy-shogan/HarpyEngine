/*
 * raven_part.cpp
 *
 *  Created on: 13 Sept 2023
 *      Author: hexi
 */

#include <raven_part.h>

namespace harpy::raven_part{
	void scene_source::load_scene(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, tinygltf::Model model, uint32_t scene_id){
				const tinygltf::Scene& scene = model.scenes[model.defaultScene];
				std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> transfer_queue = r_context_ptr->spinal_cord->queue_supervisor.lock_grab(VK_QUEUE_TRANSFER_BIT);
				struct load_package pack{r_context_ptr, transfer_queue.first.first, transfer_queue.first.second};
				std::cout<<"load_scene 1"<<std::endl;
				struct preload_map preload_map{};
				std::cout<<"load_scene 1.1"<<std::endl;
				std::cout<<model.scenes.size()<<std::endl;
				for (size_t i = 0; i < scene.nodes.size(); ++i) {
					std::cout<<"load_scene 1.1.1"<<std::endl;
					assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
					std::cout<<"load_scene 1.1.2"<<std::endl;
					loadNode(model, model.nodes[scene.nodes[i]], preload_map, pack);
				}
				std::cout<<"load_scene 2"<<std::endl;
				//r_init(model, prim, pack);
			}

	void scene_source::entity_load_camera_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node, struct load_package pack){
				human_part::ECS::Camera* cm = new human_part::ECS::Camera();
				tinygltf::Camera* cam = &model.cameras[node.camera];

				raven_part::resource_types::View view{};
				view.view_field = glm::vec2{1.0f,1.0f};
				view.cameraType = raven_part::resource_types::View::CameraType::ORTHOGRAPHIC;
				view.viewport.x = 0.0f;
				view.viewport.y = 0.0f;
				view.viewport.width = static_cast<float>(pack.r_context_ptr->swapchain.extent.width);
				view.viewport.height = static_cast<float>(pack.r_context_ptr->swapchain.extent.height);
				view.viewport.minDepth = 0.0f;
				view.viewport.maxDepth = 1.0f;

				VkRect2D scissor{};
				view.scissor.offset = {0, 0};
				view.scissor.extent = pack.r_context_ptr->swapchain.extent;

				storage.register_view(view);
				(*entities)[entity_id]->add_component(cm);
			}

}

