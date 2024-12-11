#pragma once
#ifndef HARPY_NATIVE_WINDOWING
#define HARPY_NATIVE_WINDOWING
#include <GLFW/glfw3.h>
#include <string>
#include <utilities/util/delegate.h>
#include <utilities/images/image.h>


namespace harpy::nest::windowing
{
    struct window_create_info
    {
        uint32_t width{800};
        uint32_t height{600};
        uint32_t refresh_rate{60};
        std::pair<uint32_t, uint32_t> aspect_ratio{16, 9};
        sz::string name{"BEST WINDOW EVER"};
        bool is_fullscreen{false};
        bool is_borderless{false};
        bool is_resizable{true};
        bool is_focused_on_show{true};
        bool is_visible{true};
        bool is_decorated{true};
        bool is_always_on_top{false};
        bool is_centered_cursor{false};
        bool is_transparent{false};
        //Really strange thing, allows mouse events to pass to the windows behinds
        bool mouse_passthrough{false};
    };
    
    class window
    {
        GLFWwindow* win{};
        std::array<GLFWimage, 2> images{};
        window_create_info ci{};

        //TODO: better design maybe?
        //TODO: assign parametric delegates
        utilities::delegate on_window_close_delegate{};
        utilities::delegate on_window_move_delegate{};
        utilities::delegate on_window_resize_delegate{};
        utilities::delegate on_framebuffer_resize_delegate{};
        utilities::delegate on_window_minimisation_delegate{};
        utilities::delegate on_window_maximisation_delegate{};
        utilities::delegate on_window_restoration_delegate{};
        utilities::delegate on_window_focus_delegate{};
        utilities::delegate on_window_unfocus_delegate{};


        void init();
        void bind_callbacks();
        void set_hints(window_create_info& ci);
        void window_close_callback(GLFWwindow* window);
        void window_resize_callback(GLFWwindow* window, int width, int height);
        void framebuffer_resize_callback(GLFWwindow* window, int width, int height);
        void window_moved_callback(GLFWwindow* window, int x, int y);
        void window_minimised_callback(GLFWwindow* window, int minimised);
        void window_maximised_callback(GLFWwindow* window, int maximised);
        void window_focused_callback(GLFWwindow* window, int focused);

    public:
        window(window_create_info ci);

        window(window& win);
        window(window&& win);

        window& operator=(window& win);
        window& operator=(window&& win);

        GLFWwindow*& get_glfw_window();
        operator GLFWwindow*&();

        std::pair<int, int> get_size();
        uint32_t& get_width();
        uint32_t& get_height();

        uint32_t get_position_x();
        uint32_t get_position_y();

        float get_opacity();

        void resize(int width, int height);
        void resize(std::pair<int, int> size);

        void set_fullscreen();
        void set_borderless();

        void set_windowed(int width, int height, int refresh_rate = 0);
        void set_windowed(std::pair<int, int> size, int refresh_rate = 0);

        void set_window_size_limits(std::initializer_list<int> values);
        void set_window_size_limits(int min_width, int min_height, int max_width, int max_height);
        void reset_window_size_limits();

        void set_aspect_ratio(int numer, int denom);

        void set_window_position(int x, int y);

        void set_window_title(sz::string_view title);

        void set_window_image(utilities::image first, utilities::image second = {});
        void reset_window_image();

        void set_opacity(float opacity);
        void set_decoration(bool decor);
        void set_resizability(bool resize);
        void set_always_on_top(bool top);;
        void set_focus_on_show(bool focus);
        void set_mouse_passthrough(bool mouse);
        void set_transparent(bool trans);

        //TODO: better delegate design
        void set_window_close_delegate(utilities::delegate delegate);
        void set_window_resize_delegate(utilities::delegate delegate);
        void set_window_move_delegate(utilities::delegate delegate);
        void set_framebuffer_resize_delegate(utilities::delegate delegate);
        void set_minimised_delegate(utilities::delegate delegate);
        void set_maximised_delegate(utilities::delegate delegate);
        void set_restored_delegate(utilities::delegate delegate);
        void set_focused_delegate(utilities::delegate delegate);
        void set_unfocus_delegate(utilities::delegate delegate);

        utilities::delegate& get_window_close_delegate();
        utilities::delegate& get_window_resize_delegate();
        utilities::delegate& get_window_move_delegate();
        utilities::delegate& get_framebuffer_resize_delegate();
        utilities::delegate& get_minimised_delegate();
        utilities::delegate& get_maximised_delegate();
        utilities::delegate& get_restored_delegate();
        utilities::delegate& get_focused_delegate();
        utilities::delegate& get_unfocus_delegate();

        void minimize();
        void maximise();
        void restore();

        void hide();
        void show();
        void focus();
        void request_focus();
        void close();

        bool is_minimised();
        bool is_maximised();
        bool is_windowed();
        bool is_visible();
        bool is_focused();
        bool is_transparent();
        bool is_mouse_passthrough();

        std::pair<double, double> get_screen_center();

        ~window();
    };
    
}



#endif