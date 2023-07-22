/*
 * raven_part.h
 *
 *  Created on: Jul 10, 2023
 *      Author: hexi
 */

#ifndef RAVEN_PART_RAVEN_PART_H_
#define RAVEN_PART_RAVEN_PART_H_

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>


#include <ECS/ECS.h>
#include <spinal_cord/vulkan_spinal_cord.h>
#include <renderer_context/renderer_context.h>
//using namespace harpy::human_part::ECS;
namespace harpy::raven_part{
	struct scene_source{
		std::mutex lock;
		std::atomic_flag consumed;
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities;
		human_part::ECS::Entity* camera;
		scene_source() = default;
	};
}



#endif /* RAVEN_PART_RAVEN_PART_H_ */
