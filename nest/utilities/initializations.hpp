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
#include <array>
#include <set>
#include <cstdint>

//GLFW libraries
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//Defines goes here
#define APP_VERSION VK_MAKE_API_VERSION(0,0,1,0)
#define ENGINE_VERSION VK_MAKE_API_VERSION (0, 0, 1, 0)
#define API_VERSION VK_MAKE_API_VERSION(0, 0, 1, 0)
#define ENGINE_NAME "Harpy's engine"

#define ERR_LINE (" Line: " + std::to_string(__LINE__))


#define INTCAST(x) static_cast<int>(x)

#ifdef ENABLE_VALIDATION_LAYERS
#define VALIDATION_LAYERS true
#else
#define VALIDATION_LAYERS false
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

#endif //HARPY_INITS