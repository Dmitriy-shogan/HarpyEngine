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

namespace harpy::nest
{

    struct renderer_mappings {
        uint32_t shader_id;
        uint32_t material_id;
        uint32_t pipeline_id;
        uint32_t object_id;
    };

    struct RendererObjectMapper {
        std::queue<uint32_t> recycle_queue;
        std::vector<renderer_mappings> mappings;
        std::mutex lock;

    public:
        RendererObjectMapper(){

        };

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
