/*
 * harpy.cpp
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */
#include "harpy.h"
#include <memory>
#include <shaders/shader_module.h>
#include <spinal_cord/vulkan_spinal_cord.h>

namespace harpy{
	std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;


//	struct UniformBufferObject {
//		    alignas(16) glm::mat4 model;
//		    alignas(16) glm::mat4 view;
//		    alignas(16) glm::mat4 proj;
//		};


void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* phys_cond){ //, std::vector<human_part::ECS::Entity*> entities
	glm::vec3 axisX(1.0f, 0.0f, 0.0f);
	glm::vec3 axisY(0.0f, 1.0f, 0.0f);
		while(phys_cond->test_and_set(std::memory_order_acquire)){


			obj_str_ptr->lock.lock();
			obj_str_ptr->consumed.clear();
			//obj_str_ptr->entities = std::make_shared<std::vector<human_part::ECS::Entity*>>();
			//obj_str_ptr->entities->push_back(entities[0]);
			//obj_str_ptr->entities->push_back(entities[1]);

			//dynamic_cast<harpy::human_part::ECS::Transform*>(obj_str_ptr->camera->get_components_by_name(harpy::human_part::ECS::Transform::name)[0])->rot = glm::angleAxis(angleRadians, axisX);

			//dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[0]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0])->rot = glm::angleAxis(angleRadians, axisY);


			//harpy::human_part::ECS::Transform* tr1 = dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[0]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);
			//tr1->pos_mat = glm::rotate(tr1->pos_mat, glm::radians(1.0f), axis);
			//harpy::human_part::ECS::Transform* tr2 = dynamic_cast<harpy::human_part::ECS::Transform*>((*obj_str_ptr->entities)[1]->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);
			//tr2->pos_mat = glm::rotate(tr2->pos_mat, glm::radians(1.0f), axis);

			obj_str_ptr->lock.unlock();
			std::this_thread::sleep_for(sleepDuration);
		}

	}







}
