/*
 * RendererObjectMapper.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_
#define RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_

#include <vector>
#include <queue>
#include <stdint.h>

namespace harpy::nest
{

	struct renderer_mappings{
		uint32_t shader_id;
		uint32_t material_id;
		uint32_t pipeline_id;
		uint32_t object_id;
	};

	struct RendererObjectMapper{
		std::queue<uint32_t> recycle_queue;
		std::vector<nest::renderer_mappings> mappings;

	public:
		RendererObjectMapper(){

		}

		uint32_t register_mapping(nest::renderer_mappings mappings);
		void deregister(uint32_t map_index);

		~RendererObjectMapper(){

		}
	};
}



#endif /* RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_ */
