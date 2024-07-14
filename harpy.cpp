/*
 * harpy.cpp
 *
 *  Created on: Jul 14, 2023
 *      Author: hexi
 */
#include "harpy.h"

#include <memory>
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



	float lastX = 400, lastY = 300;
	bool firstMouse = true;
	float yaw = -90.0f, pitch = 0.0f;

	void processInput(GLFWwindow *window, glm::vec3 &translation, float deltaTime) {
		float speed = 50.0f * deltaTime; // Скорость движения

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			translation.z -= speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			translation.z += speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			translation.x += speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			translation.x -= speed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			translation.y += speed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			translation.y -= speed;
	}

	void mouse_callback(GLFWwindow* window, double xpos, double ypos, glm::quat &orientation) {
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // обратное направление, так как y-координата идет вверх
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Ограничиваем pitch
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		// Обновляем кватернион
		glm::quat qPitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat qYaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		orientation = qYaw * qPitch;
	}

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

	int counter = 1;
	glm::vec3 translation(0.0f);
	glm::vec3 translation_delta(0.0f);
	float lastFrameTime = 0.0f;


	static std::shared_ptr<glm::quat> orientationPtr = std::make_shared<glm::quat>();
	glfwSetCursorPosCallback(r_context_ptr->connected_window_layout->glfw_window, [](GLFWwindow* window, double xpos, double ypos) {

		mouse_callback(window, xpos, ypos, *orientationPtr);
	});
	// glfwSetWindowUserPointer(r_context_ptr->connected_window_layout->glfw_window, &orientation);

	while(phys_cond->test_and_set(std::memory_order_acquire)){
		counter = (counter + 1) % (360 * 360 * 360);

		auto src_camera = obj_str_ptr->cameras[0];
		harpy::human_part::ECS::Camera* camera_comp = dynamic_cast<harpy::human_part::ECS::Camera*>(src_camera->get_components_by_name(harpy::human_part::ECS::Camera::name)[0]);

		harpy::human_part::ECS::Transform* transform_comp = dynamic_cast<harpy::human_part::ECS::Transform*>(src_camera->get_components_by_name(harpy::human_part::ECS::Transform::name)[0]);


//		float angleX = glm::radians(counter + 0.0f); // Угол поворота вокруг оси X
//		float angleY = glm::radians(0.0f); // Угол поворота вокруг оси Y
//
//		// Создание кватерниона для поворота по оси X
//		glm::quat quaternionX = glm::angleAxis(angleX, glm::vec3(1.0f, 0.0f, 0.0f));
//
//		// Создание кватерниона для поворота по оси Y
//		glm::quat quaternionY = glm::angleAxis(angleY, glm::vec3(0.0f, 1.0f, 0.0f));
//
//		// Комбинирование кватернионов для поворота по обеим осям
//		glm::quat resultQuat = quaternionY * quaternionX;

		//transform_comp->transform.rot = resultQuat;
		obj_str_ptr->lock.lock();

		float currentFrameTime = glfwGetTime();
		float deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		translation_delta = glm::vec3(0.0f);
		// Обработка ввода
		processInput(r_context_ptr->connected_window_layout->glfw_window, translation_delta, deltaTime);
		translation = translation + ((*orientationPtr) * translation_delta);
		// Обновление сцены (например, применение трансляции)
		std::cout << "Translation: (" << translation.x << ", " << translation.y << ", " << translation.z << ")" << std::endl;
		std::cout << "Orientation: (" << (*orientationPtr).x << ", " << (*orientationPtr).y << ", " << (*orientationPtr).z << ", " << (*orientationPtr).w << ")" << std::endl;

		transform_comp->transform.pos = translation;
		transform_comp->transform.rot = *orientationPtr;
		transform_comp->transform.recompute_matrix();
		// transform_comp->transform.mat = glm::rotate(,*orientationPtr) * glm::mat4_cast(*orientationPtr);
		// transform_comp->transform.decompose_matrix();
		// Обработка событий
		glfwPollEvents();

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
		std::filesystem::path currentPath;

		try {
			currentPath = std::filesystem::current_path();
			std::cout << "Текущая рабочая директория: " << currentPath << std::endl;
		} catch (const std::filesystem::filesystem_error& e) {
			std::cerr << "Ошибка: " << e.what() << std::endl;
		}
		bool res = GLTFloader.LoadASCIIFromFile(&model, &err, &warn, currentPath / dataguide.gltf_uri);

		if (!warn.empty()) {
			std::cout<<warn<<std::endl;
		}

		if (!err.empty()) {
			std::cout<<err<<std::endl;
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
		//TODO ЗАГЛУШКА
		current_unit->render_thread.join();

	}
}

}
