/*
 * RendererObjectMapper.h
 *
 *  Created on: Jul 9, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_
#define RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_


#include <stdint.h>
#include <queue>
#include <vector>
#include <mutex>

#include <human_part.h>
#include <utilities/initializations.h>
#include <resource_types/Material.h>
#include <resource_types/Shape.h>
#include <resource_types/View.h>

namespace harpy::nest
{

	struct RendererResourceStorage{
		std::vector<harpy::raven_part::resource_types::View> views{};
		std::vector<harpy::raven_part::resource_types::Material> materials{};
		std::vector<harpy::raven_part::resource_types::Shape> shapes{};

		void r_init(std::shared_ptr<harpy::nest::renderer_context> r_context);

		uint32_t get_vert_max();

		uint32_t register_view(raven_part::resource_types::View view);

		uint32_t register_shape(raven_part::resource_types::Shape shape);

		uint32_t register_material(raven_part::resource_types::Material material);


	};


    struct renderer_mappings {
        //uint32_t shader_id;
        uint32_t material_id;
        uint32_t pipeline_id;
        uint32_t shape_id;
    };

    struct RendererObjectMapper {
        std::queue<uint32_t> recycle_queue;
        std::vector<renderer_mappings> mappings;
        std::mutex lock;

    public:
        RendererObjectMapper(){

        }


        void register_renderer(human_part::ECS::Renderer* renderer, renderer_mappings mappings){
        			renderer->mapping_id = register_mapping(mappings);
        		}

        uint32_t register_mapping(renderer_mappings mapping) {
            if (!recycle_queue.empty()) {
                uint32_t index = recycle_queue.front();
                recycle_queue.pop();
                mappings[index] = mapping;
                return index;
            } else {
                mappings.push_back(mapping);
                return mappings.size() - 1;
            }
        }

        void deregister(uint32_t map_index) {
            if (map_index < mappings.size()) {
                recycle_queue.push(map_index);
            }
        }

        uint32_t lock_register_mapping(renderer_mappings mapping) {
			std::lock_guard<std::mutex> _(lock);
			if (!recycle_queue.empty()) {
				uint32_t index = recycle_queue.front();
				recycle_queue.pop();
				mappings[index] = mapping;
				return index;
			} else {
				mappings.push_back(mapping);
				return mappings.size() - 1;
			}
		}

		void lock_deregister(uint32_t map_index) {
			std::lock_guard<std::mutex> _(lock);
			if (map_index < mappings.size()) {
				recycle_queue.push(map_index);
			}
		}

        ~RendererObjectMapper(){

        };
    };

}



#endif /* RAVEN_PART_NEST_RENDERER_CONTEXT_RENDEREROBJECTMAPPER_H_ */
