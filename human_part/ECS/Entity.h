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
#include <string>
#include <map>

namespace harpy::human_part::ECS{
class Entity {
		std::map<std::string,std::vector<Component *>,std::less<std::string>> components{};
	public:
		Entity();
		//map indexing by const string Component.Name
		void add_component(Component * c);

		const std::vector<Component*> get_components_by_name(std::string name);

		virtual ~Entity();
	};
}

#endif /* HUMAN_PART_ECS_ENTITY_H_ */
