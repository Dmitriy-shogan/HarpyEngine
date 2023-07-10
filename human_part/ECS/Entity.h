/*
 * Entity.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_ENTITY_H_
#define HUMAN_PART_ECS_ENTITY_H_
#include <ECS/Component.h>
#include <vector>

namespace harpy::human_part::ECS{
class Entity {
		std::vector<Component> components;
	public:
		Entity();

		void add(Component c);

		virtual ~Entity();
	};
}

#endif /* HUMAN_PART_ECS_ENTITY_H_ */
