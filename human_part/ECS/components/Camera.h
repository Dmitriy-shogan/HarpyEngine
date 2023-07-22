/*
 * Camera.h
 *
 *  Created on: Jul 20, 2023
 *      Author: hexi
 */
#pragma once
#ifndef RAVEN_PART_RESOURCE_TYPES_CAMERA_H_
#define RAVEN_PART_RESOURCE_TYPES_CAMERA_H_

#include <ECS/Component.h>;
#include <bias/vertex.h>


namespace harpy::human_part::ECS{

	class Camera: public Component {

	public:
		uint32_t view_id;
		static constexpr std::string name = "Camera";
		Camera(){};
		~Camera(){};
		std::string Name() override {return name;}
	};
}



#endif /* RAVEN_PART_RESOURCE_TYPES_CAMERA_H_ */
