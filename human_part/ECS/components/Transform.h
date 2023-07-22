/*
 * Transform.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_
#define HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_

#include <ECS/Component.h>
#include <string>
#include <spinal_cord/vulkan_spinal_cord.h>
namespace harpy::human_part::ECS{
	class Transform: public Component {
	public:
		static constexpr std::string name = "Transform";

		glm::mat4 rot_mat;
		glm::vec4 pos_vec;

		Transform(glm::vec4 pos_vec, glm::mat4 rot_mat){
			this->rot_mat = rot_mat;
			this->pos_vec = pos_vec;
		};

		Transform(glm::vec4 pos_vec){
			this->rot_mat = glm::mat4(1);
			this->pos_vec = pos_vec;
		};

		Transform(){
			this->rot_mat = glm::mat4(1);
			this->pos_vec = glm::vec4(0);
		};

		~Transform(){};
		std::string Name() override {return name;}


		glm::mat4 rot_mat4(){
			return rot_mat;
		}

		glm::vec4 pos_vec4(){
			return pos_vec;
		}

		Transform * relative_to(Transform* camera){
			glm::mat4 res_rot = camera->rot_mat4() * rot_mat;
			glm::vec4 res_pos = camera->rot_mat4() * (pos_vec - camera->pos_vec4());
			return new Transform(res_pos,res_rot);
		}


	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_ */
