#pragma once
#ifndef HARPY_INITS
#define HARPY_INITS

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
#include <glm/glm.hpp>

//Defines goes here
#define APP_VERSION VK_MAKE_API_VERSION(0,0,1,0)
#define ENGINE_VERSION VK_MAKE_API_VERSION (0, 0, 1, 1)
#define API_VERSION VK_MAKE_API_VERSION(0, 0, 1, 1)
#define ENGINE_NAME "Harpy's engine"

//TODO: remove all ERR_LINE and use harpy_little_error
#define ERR_LINE (" Line: " + std::to_string(__LINE__))

#define MAX_FRAMES_IN_FLIGHT 2



#define INTCAST(x) static_cast<int>(x)

#ifdef ENABLE_VALIDATION_LAYERS
#define VALIDATION_LAYERS true
#else
#define VALIDATION_LAYERS true
#endif

//Enums go here

enum class harpy_hard_level_settings
{
    standard = 0,
    
};

enum class harpy_mid_level_settings
{
    standard = 0,
};


//For later use
#ifndef NO_HARPY_ENGINE_MACROS

#endif

//Here goes inline functions

inline void init_glfw()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

namespace harpy_nest
{
    struct vertex
    {
        glm::vec3 position{};
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

    static std::array<VkVertexInputAttributeDescription, 2> get_attributes_descriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions{{}};
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(vertex, position);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(vertex, color);
        

        return attribute_descriptions;
    }
}

#endif //HARPY_INITS