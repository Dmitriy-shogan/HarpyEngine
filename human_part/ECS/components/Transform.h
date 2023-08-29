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

		//glm::mat4 pos_mat;
		glm::quat rot;
		glm::vec3 pos;
		glm::vec3 scale;

		Transform(glm::vec3 pos_vec, glm::quat rot, glm::vec3 scale){
			pos = pos_vec;
			this->rot = rot;
			this->scale = scale; //not used now
		};

		Transform(glm::vec3 pos_vec){
			pos = pos_vec;
			this->rot = glm::quat{0.0f,0.0f,0.0f,1.0f};
		};

		Transform(){
			pos = glm::vec3(0.0f);
			this->rot = glm::quat{0.0f,0.0f,0.0f,1.0f};
		};

		~Transform(){};
		std::string Name() override {return name;}


		glm::mat4 pos_mat4(){
			return glm::mat4_cast(rot) * glm::translate(glm::mat4(1.0f), pos);
		}

		glm::mat4 pos_mat4_reversed(){
			return glm::mat4_cast(-rot) * glm::translate(glm::mat4(1.0f), -pos);
		}




	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_ */
