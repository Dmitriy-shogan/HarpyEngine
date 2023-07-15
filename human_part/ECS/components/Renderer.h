/*
 * Renderer.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENTS_RENDERER_H_
#define HUMAN_PART_ECS_COMPONENTS_RENDERER_H_

#include <ECS/Component.h>
#include <renderer_context/RendererObjectMapper.h>
#include <cstdint>

namespace harpy::nest{
	struct renderer_context;
	struct renderer_mappings;
}

namespace harpy::human_part::ECS{

	class Renderer: public Component {

	public:
		uint32_t mapping_id;
		static constexpr std::string name = "Renderer";
		Renderer(harpy::nest::renderer_context* r_context, harpy::nest::renderer_mappings mappings);
		~Renderer();
		std::string Name() override {return name;}
	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_RENDERER_H_ */
