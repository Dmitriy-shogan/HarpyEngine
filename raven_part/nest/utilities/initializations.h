#pragma once
#ifndef HARPY_INITS
#define HARPY_INITS

//TODO: create move and copy operators for all classes;
//TODO: reassign libraries where they are needed and only leave everywhere-use-libraries here
//STL libraries
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <optional>
#include <string>
#include <set>
#include <cstdint>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <format>
#include <functional>
#include <array>

//GLFW libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//GLM libraries
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Defines goes here
#define APP_VERSION VK_MAKE_API_VERSION(0,0,1,0)
#define ENGINE_VERSION VK_MAKE_API_VERSION (0, 0, 1, 1)
#define API_VERSION VK_MAKE_API_VERSION(0, 0, 1, 1)
#define ENGINE_NAME "Harpy's engine"

#define MAX_FRAMES_IN_FLIGHT 2

#define SHADER_PATH_BASE_VERTEX "shaders/base/vertex/base.vert"
#define SHADER_PATH_BASE_FRAGMENT "shaders/base/fragment/base.frag"
#define SHADER_PATH_BASE_GEOMETRY "shaders/base/geometry/base.geom"
#define SHADER_PATH_BASE_COMPUTE "shaders/base/compute/base.comp"
#define SHADER_PATH_BASE_TESSALATION_CONTROL "shaders/base/tess_control/base.tesc"
#define SHADER_PATH_BASE_TESSALATION_EVAL "shaders/base/tess_eval/base.tese"



#define INTCAST(x) static_cast<int>(x)

#ifdef ENABLE_VALIDATION_LAYERS
#define VALIDATION_LAYERS true
#else
#define VALIDATION_LAYERS true
#endif

//Enums go here


//Here goes inline functions

inline void init_glfw()
{
    glfwInit();
    

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

namespace harpy::nest
{
    struct vertex
    {
        glm::vec2 position{};
        /*glm::vec3 normal{};*/
        glm::vec3 color{};
    };

    static VkVertexInputBindingDescription get_binding_description()
    {
        VkVertexInputBindingDescription res;
        res.binding = 0;
        res.stride = sizeof(vertex);
        res.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return res;
    }

    struct needed_queues_families
    {
        //Graphic family for render itself
        std::optional<uint32_t> graphics_families;

        //Graphic family for displaying
        std::optional<uint32_t> present_families;

        bool is_completed() const
        {
            return graphics_families.has_value() && present_families.has_value();
        }
    };

    static std::array<VkVertexInputAttributeDescription, 2> get_attributes_descriptions() {
        //TODO: added normals, so rewrite shaders
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{{}};
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(vertex, position);

        /*attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(vertex, normal);*/

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(vertex, color);
        

        return attribute_descriptions;
    }

    struct ubo
    {
       alignas(16) glm::mat4 model;
       alignas(16) glm::mat4 view;
       alignas(16) glm::mat4 projection;
    };

    static  glm::mat4  projection{1.0f};

    static void change_projection(float angle, float aspect = 16.0f/9.0f, float near = 0.05f, float far = 50.0f)
    {
        projection = glm::perspective(glm::radians(angle), aspect, near, far);
        projection[1][1] *= -1;
    }
}

#endif //HARPY_INITS