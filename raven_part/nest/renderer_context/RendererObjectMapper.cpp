/*
 * RendererObjectMapper.cpp
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */

#include "RendererObjectMapper.h"
#include <raven_part.h>

uint32_t RendererResourceStorage::register_view(raven_part::resource_types::View view){

	views.push_back(view);
	return views.size()-1;
}

uint32_t RendererResourceStorage::register_shape(raven_part::resource_types::Shape shape){

	shapes.push_back(shape);
	return shapes.size()-1;
}

uint32_t RendererResourceStorage::register_material(raven_part::resource_types::Material material){

	materials.push_back(material);
	return materials.size()-1;
}

void RendererResourceStorage::r_init(renderer_context* r_context){
	for (uint32_t i = 0; i < views.size(); ++i) {
		views[i].r_init(r_context);
	}

	for (uint32_t i = 0; i < shapes.size(); ++i) {
		shapes[i].r_init(r_context);
	}

	for (uint32_t i = 0; i < materials.size(); ++i) {
		materials[i].r_init(r_context);
	}

}

uint32_t RendererResourceStorage::get_vert_max(){
	uint32_t max = 0;
	for (int i = 0; i < shapes.size(); ++i) {
		max = std::max((uint32_t)shapes[i].vert_size, max);
	}
	return max;
}
