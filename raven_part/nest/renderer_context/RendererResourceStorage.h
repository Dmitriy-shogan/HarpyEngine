/*
 * RendererResourceStorage.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERERRESOURCESTORAGE_H_
#define RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERERRESOURCESTORAGE_H_

#include <vector>
#include <resource_types/View.h>
#include <resource_types/Material.h>
#include <resource_types/Object.h>
#include <resource_types/Pipeline.h>
#include <resource_types/Shader.h>

namespace harpy::nest
{
	struct RendererResourceStorage{
		std::vector<raven_part::resource_types::View> views;
		std::vector<raven_part::resource_types::Shader> shaders;
		std::vector<raven_part::resource_types::Material> materials;
		std::vector<raven_part::resource_types::Pipeline> pipelines;
		std::vector<raven_part::resource_types::Object> objects;


	};
}


#endif /* RAVEN_PART_NEST_RENDERER_CONTEXT_RENDERERRESOURCESTORAGE_H_ */
