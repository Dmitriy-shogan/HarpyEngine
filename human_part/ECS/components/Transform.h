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
#include "../../supply/supply.h"

namespace harpy::human_part::ECS{
	class Transform: public Component {
	public:
		static constexpr std::string name = "Transform";

		//glm::mat4 pos_mat;
		struct transform transform{};

		Transform(glm::vec3 pos_vec, glm::quat rot, glm::vec3 scale){
			struct transform tr{rot, pos_vec, scale};
			transform = {rot, pos_vec, scale};
		};

		Transform(glm::mat4 mat){
			transform = {mat};
		};

		Transform(){
			transform = {};
		};

		~Transform(){};
		std::string Name() override {return name;}

		harpy::transform to_transform(){
			return transform;
		}


	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_ */
