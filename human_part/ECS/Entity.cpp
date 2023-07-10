/*
 * Entity.cpp
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#include <ECS/Entity.h>

harpy::human_part::ECS::Entity::Entity() {
	// TODO Auto-generated constructor stub

}

void harpy::human_part::ECS::Entity::add(Component c){
	components.push_back(c);
}

harpy::human_part::ECS::Entity::~Entity() {
	// TODO Auto-generated destructor stub
}

