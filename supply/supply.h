/*
 * supply.h
 *
 *  Created on: 10 Nov 2023
 *      Author: hexi
 */

#ifndef SUPPLY_SUPPLY_H_
#define SUPPLY_SUPPLY_H_
#include <utilities/utilities.h>

namespace harpy{

	struct transform{
		glm::quat rot;
		glm::vec3 pos;
		glm::vec3 scale;

		transform(){
			this->rot = glm::quat{};
			this->pos = glm::vec3{0,0,0};
			this->scale = glm::vec3{1,1,1};
		}

		transform(glm::quat rot, glm::vec3 pos, glm::vec3 scale){
			this->rot = rot;
			this->pos = pos;
			this->scale = scale;
		}

		glm::mat4 pos_mat4(){

			return glm::mat4_cast(rot) *
					glm::translate(glm::mat4(1.0f), pos) *
					glm::scale(glm::mat4(1.0f), scale);
		}

		glm::mat4 pos_mat4_reversed(){
			return glm::mat4_cast(-rot) *
					glm::translate(glm::mat4(1.0f), -pos);
		}

		transform relative_to(const transform& other){
			transform tr = transform{};
			tr.pos = this->pos + other.pos;
			tr.scale = this->scale * other.scale;
			tr.rot = this->rot * other.rot;
			return tr;
		}

	};

}



#endif /* SUPPLY_SUPPLY_H_ */
