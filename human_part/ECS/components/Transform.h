/*
 * Transform.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_
#define HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_

#include <ECS/Component.h>
namespace harpy::human_part::ECS{
	class Transform: public Component {
	public:
		Transform();
		virtual ~Transform();
	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_TRANSFORM_H_ */
