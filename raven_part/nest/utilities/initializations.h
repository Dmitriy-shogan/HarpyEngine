#pragma once
#ifndef HARPY_INITS
#define HARPY_INITS

//STL libraries
#include <iostream>
#include <fstream>
#include <algorithm>
#include <optional>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <any>
#include <functional>
#include <array>
//libfmt
#include <fmt/core.h>
#include <fmt/ranges.h>

//GLFW libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM libraries
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gltf/tiny_gltf.h>
#include <toml++/toml.h>


//Defines goes here
#define APP_VERSION VK_MAKE_API_VERSION(0,0,1,0)
#define ENGINE_VERSION VK_MAKE_API_VERSION (0, 0, 1, 1)
#define API_VERSION VK_API_VERSION_1_3 //VK_MAKE_API_VERSION(0, 0, 1, 1)
#define ENGINE_NAME "Harpy's engine"

#define MAX_FRAMES_IN_FLIGHT 2

#define SHADER_PATH_BASE_VERTEX "shaders/base/vertex/base.vert"
#define SHADER_PATH_BASE_FRAGMENT "shaders/base/fragment/base.frag"
#define SHADER_PATH_BASE_GEOMETRY "shaders/base/geometry/base.geom"
#define SHADER_PATH_BASE_COMPUTE "shaders/base/compute/base.comp"
#define SHADER_PATH_BASE_TESSALATION_CONTROL "shaders/base/tess_control/base.tesc"
#define SHADER_PATH_BASE_TESSALATION_EVAL "shaders/base/tess_eval/base.tese"

#define SHADER_PATH_BLENDER_VERTEX_DUMMY "shaders/base/vertex/service/blender.vert"
#define SHADER_PATH_BLENDER_FRAGMENT "shaders/base/compute/service/blender.frag"

#define SHADER_PATH_CAMERA_PRESPECTIVE_VERTEX "shaders/base/compute/service/camera_perespective.comp"
#define SHADER_PATH_CAMERA_ORTHOGRAPHIC_VERTEX "shaders/base/compute/service/camera_orthographic.comp"



#define RENDERER_MEMORY_OPTI_POLICY_PASSIVE 0
#define RENDERER_MEMORY_OPTI_POLICY_AGRESSIVE 1

#define RENDERER_MEMORY_OPTI_POLICY RENDERER_MEMORY_OPTI_POLICY_PASSIVE

//#define PRELOAD_STRATEGY_FULL 0
//#define PRELOAD_STRATEGY_PART 1
//
//#define PRELOAD_STRATEGY PRELOAD_STRATEGY_FULL



#define INTCAST(x) static_cast<int>(x)

//#ifdef ENABLE_VALIDATION_LAYERS
//#define VALIDATION_LAYERS true
//#else
//#define VALIDATION_LAYERS false
//#endif
#define VALIDATION_LAYERS true
//Enums go here


//Here goes inline functions

inline void init_glfw()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}


static void change_projection(float angle, float aspect = 16.0f/9.0f, float near = 0.05f, float far = 50.0f)
{
    auto projection = glm::perspective(glm::radians(angle), aspect, near, far);
    projection[1][1] *= -1;
}


#endif //HARPY_INITS
