/*
 * Entity.cpp
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#include <ECS/Entity.h>
#include <ECS/Component.h>
#include <vector>

harpy::human_part::ECS::Entity::Entity() {}

void harpy::human_part::ECS::Entity::add_component(harpy::human_part::ECS::Component * c) {
    components[c->Name()].push_back(c);
}

const std::vector<harpy::human_part::ECS::Component*> harpy::human_part::ECS::Entity::get_components_by_name(std::string name) {
	std::vector<harpy::human_part::ECS::Component*> ptrs;
	if (components.count(name) > 0) {
        return components[name];
    }
    return ptrs;
}

harpy::human_part::ECS::Entity::~Entity() {
    for (auto& pair : components) {
        for (Component * component : pair.second) {
            delete component;
        }
    }
}
