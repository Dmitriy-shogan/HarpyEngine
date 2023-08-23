/*
 * RendererObjectMapper.cpp
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */

#include "RendererObjectMapper.h"


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

void RendererResourceStorage::r_init(std::shared_ptr<harpy::nest::renderer_context> r_context){
	for (uint32_t i = 0; i < views.size(); ++i) {
		views[i].init(r_context);
	}

	for (uint32_t i = 0; i < shapes.size(); ++i) {
		shapes[i].init(r_context);
	}

	for (uint32_t i = 0; i < materials.size(); ++i) {
		materials[i].init(r_context);
	}

}

uint32_t RendererResourceStorage::get_vert_max(){
	uint32_t max = 0;
	for (int i = 0; i < shapes.size(); ++i) {
		max = std::max((uint32_t)shapes[i].vert_size, max);
	}
	return max;
}
