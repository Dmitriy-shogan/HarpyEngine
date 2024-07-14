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
		glm::mat4 mat;
		glm::quat rot;
		glm::vec3 pos;
		glm::vec3 scale;

		transform(){
			this->mat = glm::mat4(1.0f);
			decompose_matrix();
		}

		transform(glm::mat4 mat){
					this->mat = mat;
					decompose_matrix();
				}

		transform(glm::quat rot, glm::vec3 pos, glm::vec3 scale){
			this->pos = pos;
			this->rot = rot;
			this->scale = scale;
			recompute_matrix();
		}

		glm::mat4 pos_mat4(){

			return mat;
		}

		glm::mat4 pos_mat4_reversed(){

			return glm::inverse(mat);
		}

		transform relative_to(transform& base) {

            transform tr(base.mat * mat);

            return tr;
        }

		void recompute_matrix()
		{
			this->mat = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(glm::normalize(rot)) * glm::scale(glm::mat4(1.0f), scale);
		}

		void decompose_matrix()
		{
			glm::vec3 skew;
			glm::vec4 prespective;
			glm::decompose(this->mat, this->scale, this->rot, this->pos, skew, prespective);
		}

//		transform relative_to(transform& base) {
//			transform tr;
//			glm::mat4 matrix = base.pos_mat4() * this->pos_mat4();
//			glm::vec3 skew;
//			glm::vec4 prespective;
//			glm::decompose(matrix, tr.scale, tr.rot, tr.pos, skew, prespective);
//			return tr;
//		}

	};


}



#endif /* SUPPLY_SUPPLY_H_ */
