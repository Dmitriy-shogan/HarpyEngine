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
#include <bias/vertex.h>
#include <memory>
#include <future>


namespace harpy{

	const std::string HARPY_DATAGUIDE_URI = "harpy.dataguide";

	struct dataguide{
		std::string gltf_uri;
	};

	struct harpy_state{
		uint32_t scene_id;
	};

	void render(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::atomic_flag* cond);
	void physics(std::shared_ptr<harpy::raven_part::scene_source> obj_str_ptr, std::vector<human_part::ECS::Entity*> entities, std::atomic_flag* cond);

	struct scene_workaround_unit{
		std::thread physics_thread;
		std::thread render_thread;
		std::atomic_flag rend_cond{true};
		std::atomic_flag phys_cond{true};

		std::shared_ptr<harpy::raven_part::scene_source> scene_source{};

		std::shared_ptr<renderer_context> r_context_ptr;

		scene_workaround_unit(std::shared_ptr<renderer_context> r_context_ptr){
			this->r_context_ptr = r_context_ptr;
		}


		void start(){
			rend_cond.test_and_set();
			phys_cond.test_and_set();

			physics_thread = std::thread(physics,scene_source,&phys_cond);
			render_thread = std::thread(render, r_context_ptr, scene_source, &rend_cond);

		}

		void stop(){
			rend_cond.clear();
			phys_cond.clear();

			physics_thread.join();
			render_thread.join();
		}

		void set_scene(std::shared_ptr<harpy::raven_part::scene_source> scene_source){
			this->scene_source = scene_source;
		}

	};

	struct scene_manager{
		std::shared_ptr<scene_workaround_unit> current_unit;
		std::map<uint32_t, std::shared_ptr<scene_source>> scenes;
		std::shared_ptr<renderer_context> r_context_ptr;

		scene_manager(std::shared_ptr<renderer_context> r_context_ptr){
			this->r_context_ptr = r_context_ptr;
		}

		void load_scene(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, tinygltf::Model model){
			harpy::raven_part::scene_source src{};
			src.load_scene(r_context_ptr, model, model.defaultScene);
			scenes[model.defaultScene] = std::make_shared<scene_source>(src);
		}

		void load_scene(std::shared_ptr<harpy::nest::renderer_context> r_context_ptr, tinygltf::Model model, uint32_t scene_id){
			harpy::raven_part::scene_source src{};
			src.load_scene(r_context_ptr, model, scene_id);
			scenes[model.defaultScene] = std::make_shared<scene_source>(src);
		}

		void start_scene(uint32_t scene_id){
			if (scenes.count(scene_id) != 0){
				std::shared_ptr<scene_workaround_unit> unit = std::make_shared<scene_workaround_unit>(scene_workaround_unit{r_context_ptr});
				unit->set_scene(scenes[scene_id]);
				current_unit = unit;
				unit->start();

			}
		}
	};

	extern std::shared_ptr<harpy::nest::renderer_context> r_context_ptr;
	const std::chrono::milliseconds sleepDuration(100);

	static struct dataguide dataguide{};
	static struct harpy_state state{};
	static tinygltf::Model model;
//	struct scene_manager scene_manager;
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

	    tinygltf::TinyGLTF loader;
	    std::string err;
	    std::string warn;

	    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, dataguide.gltf_uri);
	    if (!warn.empty()) {
	        std::cout << "WARN: " << warn << std::endl;
	    }

	    if (!err.empty()) {
	    	throw harpy::utilities::harpy_little_error("Failed to load gltf scene");
	    }

	}

	static void init(){
		load_dataguide();
		load_gltf();
		harpy::nest::vulkan_spinal_cord::init();

		std::unique_ptr<harpy::nest::base_window_layout> layout = std::make_unique<harpy::nest::base_window_layout>();
		r_context_ptr = std::make_unique<harpy::nest::renderer_context>(harpy::nest::vulkan_spinal_cord::getInstance(), std::move(layout));
		r_context_ptr->init();

		scene_manager scene_manager{r_context_ptr};

	}
	static void run(){
		std::shared_ptr<vulkan_spinal_cord> cord =  vulkan_spinal_cord::getInstance();
		//std::pair<std::pair<VkQueue, VkCommandBuffer>, uint32_t> transfer_queue = cord->queue_supervisor.lock_grab(VK_QUEUE_TRANSFER_BIT);

		std::cout<<"дычапщха"<<std::endl;

	}



}

#endif /* HARPY_H_ */
