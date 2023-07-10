#pragma once
#ifndef HAPRY_DELTA_TIMER
#define HARPY_DELTA_TIMER
#include <utilities/harpy_little_error.h>
#include <utilities/property.h>
#include <utilities/utilities.h>
#include <functional>

namespace harpy::utilities {
    
class delta_timer
{
    float delta{};
    float last_time{static_cast<float>(glfwGetTime())};
    float now_time{static_cast<float>(glfwGetTime())};
    float get_delta_time()
    {
        solve_delta();
        return delta;
    }
    void solve_delta()
    {
        now_time = static_cast<float>(glfwGetTime());
        delta = now_time - last_time;
        last_time = now_time;
    }
public:

    delta_timer(GLFWwindow* window)
    {
        now_time = static_cast<float>(glfwGetTime());
        delta = now_time - last_time;
        last_time = now_time;
        delta_time.apply_object(this);
        delta_time.apply_getter_property(method<float, delta_timer>(&delta_timer::get_delta_time));
    }
    property<delta_timer, float, access::read> delta_time;
};

}



#endif //HARPY_DELTA_TIMER
