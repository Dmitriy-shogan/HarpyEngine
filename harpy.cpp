/*
 * harpy.cpp
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */
#include "harpy.h"
#include <memory>
#include <shaders/shader_module.h>
#include <spinal_cord/vulkan_spinal_cord.h>

namespace harpy{
	std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;
	struct dataguide dataguide{};
	struct harpy_state state{};
	tinygltf::TinyGLTF GLTFloader;
	tinygltf::Model model;
	struct scene_manager sceneManager;

void render(
		std::shared_ptr<harpy::nest::renderer_context> r_context_ptr,
		std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr,
		std::atomic_flag* cond
		){
	r_context_ptr->render_loop(obj_str_ptr, cond);
};

void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* phys_cond){ //, std::vector<human_part::ECS::Entity*> entities
	glm::vec3 axisX(1.0f, 0.0f, 0.0f);
	glm::vec3 axisY(0.0f, 1.0f, 0.0f);
	while(phys_cond->test_and_set(std::memory_order_acquire)){
		obj_str_ptr->lock.lock();
		obj_str_ptr->consumed.clear();
		obj_str_ptr->lock.unlock();
		std::this_thread::sleep_for(sleepDuration);
	}
}

void load_dataguide(){

	toml::table tbl;
	try
	{
		tbl = toml::parse_file(HARPY_DATAGUIDE_URI);
		dataguide.gltf_uri = *tbl["gltf"]["uri"].value<std::string>();
	}
	catch (const toml::parse_error& err)
	{
		throw harpy::utilities::harpy_little_error("Failed to load dataguide");
	}

}

void load_gltf(){
		std::string err;
		std::string warn;

		bool res = GLTFloader.LoadASCIIFromFile(&model, &err, &warn, dataguide.gltf_uri);
		std::cout << "Scenes count: " << model.scenes.size() << std::endl;
		if (!warn.empty()) {
			std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			throw harpy::utilities::harpy_little_error("Failed to load gltf scene");
		}

	}




void scene_manager::init(std::shared_ptr<renderer_context> r_context_ptr){
			this->r_context_ptr = r_context_ptr;
		}

void scene_manager::load_scene(tinygltf::Model& model, uint32_t scene_id){
	scenes[scene_id] = std::make_shared<scene_source>();
	scenes[scene_id]->load_scene(r_context_ptr, model, scene_id);
}

void scene_manager::start_scene(uint32_t scene_id){
	if (scenes.count(scene_id) != 0){
		current_unit = std::make_shared<scene_workaround_unit>(r_context_ptr);
		current_unit->set_scene(scenes[scene_id]);

		current_unit->start();
		//ЗАГЛУШКА
		current_unit->render_thread.join();

	}
}

}
