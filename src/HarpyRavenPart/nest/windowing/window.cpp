#include <nest/windowing/window.h>

#include <logger/logger.h>

void harpy::nest::windowing::window::init() {

    set_hints(ci);
    if (ci.is_fullscreen) {
        win = glfwCreateWindow(ci.width, ci.height, ci.name.c_str(), glfwGetPrimaryMonitor(), nullptr);
    } else if (ci.is_borderless) {
        hide();
        win = glfwCreateWindow(1, 1, ci.name.c_str(), nullptr, nullptr);
        auto video_mode = glfwGetVideoMode(glfwGetWindowMonitor(win));
        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
        glfwSetWindowMonitor(win, glfwGetWindowMonitor(win), 0, 0, ci.width, ci.height, ci.refresh_rate);
        resize(ci.width, ci.height);
        show();
    } else
        win = glfwCreateWindow(ci.width, ci.height, ci.name.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(win, this);
}

harpy::nest::windowing::window::window(window_create_info ci)
{
    this->ci = ci;
    init();
}

harpy::nest::windowing::window::window(window& win)
{
    this->ci = win.ci;
    init();
}

harpy::nest::windowing::window::window(window&& win)
{
    this->ci = win.ci;
    this->win = win.win;
    win.win = nullptr;
}

harpy::nest::windowing::window& harpy::nest::windowing::window::operator=(window& win)
{
    this->ci = win.ci;
    if (!this->win) init();
    return *this;
}

harpy::nest::windowing::window& harpy::nest::windowing::window::operator=(window&& win) {
    this->ci = win.ci;
    this->win = win.win;
    win.win = nullptr;
    return *this;
}

GLFWwindow*& harpy::nest::windowing::window::get_glfw_window()
{return win;}

harpy::nest::windowing::window::operator GLFWwindow*&()
{return win;}

std::pair<int, int> harpy::nest::windowing::window::get_size()
{
    return {ci.width, ci.height};
}

uint32_t& harpy::nest::windowing::window::get_width()
{
    return ci.width;
}

uint32_t& harpy::nest::windowing::window::get_height()
{
    return ci.height;
}

void harpy::nest::windowing::window::resize(int width, int height)
{
    ci.width = width;
    ci.height = height;
    glfwSetWindowSize(win, ci.width, ci.height);
}

void harpy::nest::windowing::window::resize(std::pair<int, int> size)
{
    ci.width = size.first;
    ci.height = size.second;
    glfwSetWindowSize(win, ci.width, ci.height);
}

harpy::nest::windowing::window::~window()
{
    if(win) glfwDestroyWindow(win);
}

void harpy::nest::windowing::window::set_hints(harpy::nest::windowing::window_create_info &ci) {
    glfwWindowHint(GLFW_FOCUSED, true);
    glfwWindowHint(GLFW_FLOATING, ci.is_always_on_top);
    glfwWindowHint(GLFW_RESIZABLE, ci.is_resizable);
    glfwWindowHint(GLFW_VISIBLE, ci.is_visible);
    glfwWindowHint(GLFW_DECORATED, ci.is_decorated);
    glfwWindowHint(GLFW_CENTER_CURSOR, ci.is_centered_cursor);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, ci.is_transparent);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR,true);
    glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, ci.mouse_passthrough);
}

void harpy::nest::windowing::window::window_close_callback(GLFWwindow *window) {
    on_window_close_delegate.invoke(false);
}

bool harpy::nest::windowing::window::is_windowed() {
    return glfwGetWindowMonitor(win);
}

bool harpy::nest::windowing::window::is_minimised() {
    return glfwGetWindowAttrib(win, GLFW_ICONIFIED);
}

bool harpy::nest::windowing::window::is_maximised() {
    return glfwGetWindowAttrib(win, GLFW_MAXIMIZED);
}

bool harpy::nest::windowing::window::is_visible() {
    return glfwGetWindowAttrib(win, GLFW_VISIBLE);
}

bool harpy::nest::windowing::window::is_focused() {
    return glfwGetWindowAttrib(win, GLFW_FOCUSED);
}

void harpy::nest::windowing::window::request_focus() {
    glfwRequestWindowAttention(win);
}

bool harpy::nest::windowing::window::is_transparent() {
    return glfwGetWindowAttrib(win, GLFW_TRANSPARENT_FRAMEBUFFER);
}

bool harpy::nest::windowing::window::is_mouse_passthrough() {
    return glfwGetWindowAttrib(win, GLFW_MOUSE_PASSTHROUGH);
}

float harpy::nest::windowing::window::get_opacity() {
    return glfwGetWindowOpacity(win);
}

void harpy::nest::windowing::window::set_fullscreen() {
    glfwWindowHint(GLFW_RED_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_GREEN_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_BLUE_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

    auto video_mode = glfwGetVideoMode(glfwGetWindowMonitor(win));
    glfwSetWindowMonitor(win, glfwGetWindowMonitor(win), 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
}

void harpy::nest::windowing::window::set_borderless() {
    auto video_mode = glfwGetVideoMode(glfwGetWindowMonitor(win));

    glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);
    glfwSetWindowMonitor(win, glfwGetWindowMonitor(win), 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
}

void harpy::nest::windowing::window::set_windowed(int width, int height, int refresh_rate) {
    glfwWindowHint(GLFW_RED_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_GREEN_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_BLUE_BITS, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

    glfwSetWindowMonitor(win, glfwGetWindowMonitor(win), 0, 0, width, height, refresh_rate ? refresh_rate : glfwGetVideoMode(glfwGetWindowMonitor(win))->refreshRate);
}

void harpy::nest::windowing::window::set_windowed(std::pair<int, int> size, int refresh_rate) {
    set_windowed(size.first, size.second, refresh_rate);
}

void harpy::nest::windowing::window::set_window_size_limits(std::initializer_list<int> values) {
    if(values.size() < 4)
        utilities::logger::get_logger() << "Limits on window wasn't applied because not enough values";

    glfwSetWindowSizeLimits(win, values.begin()[0], values.begin()[1], values.begin()[2], values.begin()[3]);
}

void
harpy::nest::windowing::window::set_window_size_limits(int min_width, int min_height, int max_width, int max_height) {
    glfwSetWindowSizeLimits(win, min_width, min_height, max_width, max_height);
}

void harpy::nest::windowing::window::reset_window_size_limits() {
    glfwSetWindowSizeLimits(win, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
}

void harpy::nest::windowing::window::set_aspect_ratio(int numer, int denom) {
    glfwSetWindowAspectRatio(win, numer, denom);
}

void harpy::nest::windowing::window::set_window_position(int x, int y) {
    glfwSetWindowPos(win, x, y);
}

void harpy::nest::windowing::window::set_window_title(std::string title) {
    glfwSetWindowTitle(win, title.c_str());
}

void harpy::nest::windowing::window::set_window_title(std::u8string title) {
    glfwSetWindowTitle(win, reinterpret_cast<const char*>(title.c_str()));
}

void harpy::nest::windowing::window::set_window_image(harpy::utilities::image first,
                                                      harpy::utilities::image second) {
    if(first.get_cv_data().empty())
        throw utilities::harpy_little_error("Oh, you tried to put empty image as window icon. Sad");

    //TODO: Make this shit normal
    images[0].pixels = first.get_cv_data().data;
    images[0].height = first.get_cv_data().cols;
    images[0].width = first.get_cv_data().rows;

    if(!second.get_cv_data().empty()) {
        if(second.get_current_format() != utilities::image_format::RGBA)
            second.convert_image_color_space(cv::COLOR_BGR2RGBA);

        images[1].pixels = first.get_cv_data().data;
        images[1].pixels = first.get_cv_data().data;
        images[1].pixels = first.get_cv_data().data;
    }

    glfwSetWindowIcon(win, second.get_cv_data().empty() ? 2 : 1, images.data());
}

void harpy::nest::windowing::window::reset_window_image() {
    glfwSetWindowIcon(win, 0, nullptr);
}

void harpy::nest::windowing::window::set_opacity(float opacity) {
    glfwSetWindowOpacity(win, opacity);
}

void harpy::nest::windowing::window::set_decoration(bool decor) {
    ci.is_decorated = decor;
    glfwSetWindowAttrib(win, GLFW_DECORATED, decor);
}

void harpy::nest::windowing::window::set_resizability(bool resize) {
    ci.is_resizable = resize;
    glfwSetWindowAttrib(win, GLFW_RESIZABLE, resize);
}

void harpy::nest::windowing::window::set_always_on_top(bool top) {
    ci.is_always_on_top = top;
    glfwSetWindowAttrib(win, GLFW_FLOATING, top);
}

void harpy::nest::windowing::window::set_focus_on_show(bool focus) {
    ci.is_focused_on_show = focus;
    glfwSetWindowAttrib(win, GLFW_FOCUS_ON_SHOW, focus);
}

void harpy::nest::windowing::window::set_mouse_passthrough(bool mouse) {
    ci.mouse_passthrough = mouse;
    glfwSetWindowAttrib(win, GLFW_MOUSE_PASSTHROUGH, mouse);
}

void harpy::nest::windowing::window::set_transparent(bool trans) {
    ci.is_transparent = trans;
    glfwSetWindowAttrib(win, GLFW_TRANSPARENT_FRAMEBUFFER, trans);
}

void harpy::nest::windowing::window::minimize() {
    glfwIconifyWindow(win);
}

void harpy::nest::windowing::window::maximise() {
    glfwSetWindowAttrib(win, GLFW_MAXIMIZED, true);
}

void harpy::nest::windowing::window::restore() {
    glfwRestoreWindow(win);
}

void harpy::nest::windowing::window::hide() {
    glfwHideWindow(win);
}

void harpy::nest::windowing::window::show() {
    glfwShowWindow(win);
}

void harpy::nest::windowing::window::focus() {
    glfwFocusWindow(win);
}

void harpy::nest::windowing::window::bind_callbacks() {
    auto callback = [](GLFWwindow* w)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_close_callback(w);
    };
    glfwSetWindowCloseCallback(win, callback);
    auto callback2 = [](GLFWwindow* w, int x, int y)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_resize_callback(w, x, y);
    };
    glfwSetWindowSizeCallback(win, callback2);
    auto callback3 = [](GLFWwindow* w, int x, int y)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_moved_callback(w, x, y);
    };
    glfwSetWindowPosCallback(win, callback3);
    auto callback4 = [](GLFWwindow* w, int x, int y)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->framebuffer_resize_callback(w, x, y);
    };
    glfwSetFramebufferSizeCallback(win, callback4);
    auto callback5 = [](GLFWwindow* w, int x)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_minimised_callback(w, x);
    };
    glfwSetWindowIconifyCallback(win, callback5);
    auto callback6 = [](GLFWwindow* w, int x)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_maximised_callback(w, x);
    };
    glfwSetWindowMaximizeCallback(win, callback6);
    auto callback7 = [](GLFWwindow* w, int x)
    {
        static_cast<window*>(glfwGetWindowUserPointer(w))->window_focused_callback(w, x);
    };
    glfwSetWindowFocusCallback(win, callback7);
}

void harpy::nest::windowing::window::close() {
    glfwSetWindowShouldClose(win, true);
}

void harpy::nest::windowing::window::window_resize_callback(GLFWwindow *window, int width, int height) {
    ci.width = width;
    ci.height = height;
    on_window_resize_delegate.invoke();
}

void harpy::nest::windowing::window::framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
    on_framebuffer_resize_delegate.invoke();
}

void harpy::nest::windowing::window::window_moved_callback(GLFWwindow *window, int x, int y) {
    on_window_move_delegate.invoke();
}

void harpy::nest::windowing::window::window_minimised_callback(GLFWwindow *window, int minimised) {
    if(minimised)
        on_window_minimisation_delegate();
    else
        on_window_restoration_delegate();
}

void harpy::nest::windowing::window::window_maximised_callback(GLFWwindow *window, int maximised) {
    if(maximised)
        on_window_maximisation_delegate();
    else
        on_window_restoration_delegate();

}

void harpy::nest::windowing::window::window_focused_callback(GLFWwindow *window, int focused) {
    if(focused) on_window_focus_delegate();
    else on_window_unfocus_delegate();
}

void harpy::nest::windowing::window::set_window_close_delegate(harpy::utilities::delegate delegate) {
    on_window_close_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_window_resize_delegate(harpy::utilities::delegate delegate) {
    on_window_resize_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_window_move_delegate(harpy::utilities::delegate delegate) {
    on_window_move_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_framebuffer_resize_delegate(harpy::utilities::delegate delegate) {
    on_framebuffer_resize_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_minimised_delegate(harpy::utilities::delegate delegate) {
    on_window_minimisation_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_maximised_delegate(harpy::utilities::delegate delegate) {
    on_window_maximisation_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_restored_delegate(harpy::utilities::delegate delegate) {
    on_window_restoration_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_focused_delegate(harpy::utilities::delegate delegate) {
    on_window_focus_delegate = std::move(delegate);
}

void harpy::nest::windowing::window::set_unfocus_delegate(harpy::utilities::delegate delegate) {
    on_window_unfocus_delegate = std::move(delegate);
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_window_close_delegate() {
    return on_window_close_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_window_resize_delegate() {
    return on_window_resize_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_window_move_delegate() {
    return on_window_move_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_framebuffer_resize_delegate() {
    return on_framebuffer_resize_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_minimised_delegate() {
    return on_window_minimisation_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_maximised_delegate() {
    return on_window_maximisation_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_restored_delegate() {
    return on_window_restoration_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_focused_delegate() {
    return on_window_focus_delegate;
}

harpy::utilities::delegate &harpy::nest::windowing::window::get_unfocus_delegate() {
    return on_window_unfocus_delegate;
}

uint32_t harpy::nest::windowing::window::get_position_x() {
    uint32_t x, y;
    glfwGetWindowPos(win, reinterpret_cast<int*>(&x), reinterpret_cast<int*>(&y));
    return x;
}

uint32_t harpy::nest::windowing::window::get_position_y() {
    uint32_t x, y;
    glfwGetWindowPos(win, reinterpret_cast<int*>(&x), reinterpret_cast<int*>(&y));
    return y;
}
