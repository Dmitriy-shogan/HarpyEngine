/*
 * Renderer.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENTS_RENDERER_H_
#define HUMAN_PART_ECS_COMPONENTS_RENDERER_H_

#include <ECS/Component.h>

#include <cstdint>



namespace harpy::human_part::ECS{

	class Renderer: public Component {

	public:
		std::vector<uint32_t> mappings;
		static constexpr std::string name = "Renderer";
		Renderer();
		~Renderer();
		std::string Name() override {return name;}
	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_RENDERER_H_ */
