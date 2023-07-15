/*
 * harpy.h
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */

#ifndef HARPY_H_
#define HARPY_H_

#include <raven_part.h>
#include <human_part.h>
#include <memory>
namespace harpy{
	extern std::unique_ptr<harpy::nest::renderer_context> r_context_ptr;
	static void init(){
		harpy::nest::vulkan_spinal_cord::init();
	}
	static void run(){
		std::unique_ptr<harpy::nest::base_window_layout> layout = std::make_unique<harpy::nest::base_window_layout>();


		std::atomic_flag rend_cond{true};
		r_context_ptr = std::make_unique<harpy::nest::renderer_context>(harpy::nest::vulkan_spinal_cord::getInstance(), std::move(layout));
		std::cout<<"4"<<std::endl;
		r_context_ptr->init();
		std::cout<<"5"<<std::endl;
		std::shared_ptr<harpy::raven_part::object_source> obj_str_ptr= std::make_shared<harpy::raven_part::object_source>();
		obj_str_ptr->entities = std::make_shared<std::vector<human_part::ECS::Entity*>>();
		r_context_ptr->render_loop(obj_str_ptr, &rend_cond);

		std::cout<<"дычапщха"<<std::endl;

	}

}

#endif /* HARPY_H_ */
