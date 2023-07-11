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

#include <ECS/Entity.h>
//using namespace harpy::human_part::ECS;
namespace harpy::raven_part{
	struct object_source{
		std::mutex lock;
		std::atomic_flag consumed;
		std::shared_ptr<std::vector<human_part::ECS::Entity*>> entities;
		object_source() = default;
	};
}



#endif /* RAVEN_PART_RAVEN_PART_H_ */
