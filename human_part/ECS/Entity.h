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
#include <utilities/initializations.h>

namespace harpy::human_part::ECS{
class Entity {
		std::map<std::string,std::vector<Component *>,std::less<std::string>> components{};
		uint32_t parent_id;
	public:
		Entity(uint32_t parent_id) {this->parent_id = parent_id;}
		Entity() {this->parent_id = -1;}
		//map indexing by const string Component.Name
		void add_component(Component * c);

		const std::vector<Component*> get_components_by_name(std::string name);

		uint32_t get_parent_id(){
			return parent_id;
		}
		virtual ~Entity();
	};
}

#endif /* HUMAN_PART_ECS_ENTITY_H_ */
