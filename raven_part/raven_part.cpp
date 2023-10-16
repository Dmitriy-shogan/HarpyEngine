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

				struct preload_map preload_map{};



				for (size_t i = 0; i < scene.nodes.size(); ++i) {

					assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));

					loadNode(model, model.nodes[scene.nodes[i]],-1, preload_map, pack);
				}
				r_context_ptr->spinal_cord->queue_supervisor.lock_free(transfer_queue.second);

				//r_init(model, prim, pack);
			}

	void scene_source::entity_load_camera_component(uint32_t entity_id, tinygltf::Model& model, tinygltf::Node& node, struct load_package pack){
				human_part::ECS::Camera* cm = new human_part::ECS::Camera();
				tinygltf::Camera* cam = &model.cameras[node.camera];

				//TODO setup_camera
				raven_part::resource_types::View view{};
				view.view_field = glm::vec2{1.0f,1.0f};

				view.viewport.x = 0.0f;
				view.viewport.y = 0.0f;
				view.viewport.width = static_cast<float>(pack.r_context_ptr->swapchain.extent.width);
				view.viewport.height = static_cast<float>(pack.r_context_ptr->swapchain.extent.height);
				view.viewport.minDepth = 0.0f;
				view.viewport.maxDepth = 1.0f;




				if(GLTF_CAMERA_TYPE_PERSPECTIVE){
						view.cameraType = raven_part::resource_types::View::CameraType::PERSPECTIVE;
						view.aspectRatio = cam->perspective.aspectRatio;
						view.focus = 0.5f / glm::tan(cam->perspective.yfov / 2);

				}else if (GLTF_CAMERA_TYPE_ORTHOGRAPHIC){
						view.cameraType = raven_part::resource_types::View::CameraType::ORTHOGRAPHIC;

				} else if (GLTF_CAMERA_TYPE_ISOMETRIC){
						view.cameraType = raven_part::resource_types::View::CameraType::ISOMETRIC;

				} else{
						throw utilities::harpy_little_error("fake camera type");
				}

				VkRect2D scissor{};
				view.scissor.offset = {0, 0};
				view.scissor.extent = pack.r_context_ptr->swapchain.extent;

				view.load(model, pack);
				cm->view_id = storage.register_view(view);
				(*entities)[entity_id]->add_component(cm);
			}

}

