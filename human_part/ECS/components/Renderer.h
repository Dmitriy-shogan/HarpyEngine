/*
 * Renderer.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef HUMAN_PART_ECS_COMPONENTS_RENDERER_H_
#define HUMAN_PART_ECS_COMPONENTS_RENDERER_H_

#include <ECS/Component.h>
namespace harpy::human_part::ECS{
	class Renderer: public Component {
	public:
		Renderer();
		virtual ~Renderer();
	};
}
#endif /* HUMAN_PART_ECS_COMPONENTS_RENDERER_H_ */
