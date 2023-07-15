/*
 * Renderer.cpp
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */


#include <renderer_context/renderer_context.h>
#include <renderer_context/RendererObjectMapper.h>
#include <ECS/components/Renderer.h>

harpy::human_part::ECS::Renderer::Renderer(harpy::nest::renderer_context* r_context, renderer_mappings mappings){
	mapping_id = r_context->mapper.lock_register_mapping(mappings);
}

harpy::human_part::ECS::Renderer::~Renderer() {
	// TODO Auto-generated destructor stub
}

