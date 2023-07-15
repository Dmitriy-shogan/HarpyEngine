/*
 * Component.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENT_H_
#define HUMAN_PART_ECS_COMPONENT_H_
#include <string>
namespace harpy::human_part::ECS{
	class Component {
	public:
		Component();
		virtual ~Component();
		virtual std::string Name() = 0;
	};
}
#endif /* HUMAN_PART_ECS_COMPONENT_H_ */
