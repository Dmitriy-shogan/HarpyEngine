//
// Created by Xyeveloper on 27.07.2024.
//

#include <nest/windowing/input_controller.h>

void harpy::nest::windowing::input_controller::static_input_callback_manager::set_instance(
        harpy::nest::windowing::input_controller *controller1) {
    controller = controller1;
}

harpy::nest::windowing::input_controller *
harpy::nest::windowing::input_controller::static_input_callback_manager::get_input_controller() {
    return controller;
}

void harpy::nest::windowing::input_controller::static_input_callback_manager::key_callback(GLFWwindow *window, int key,
                                                                                           int scancode, int action,
                                                                                           int mods) {
    key |= mods << 10;
    switch(action){
        case GLFW_PRESS:{
            controller->press_keymap[key].invoke();
            break;
        }
        case GLFW_RELEASE:{
            controller->release_keymap[key].invoke();
            break;
        }
        case GLFW_REPEAT:{
            controller->repeat_keymap[key].invoke();
            break;
        }
        default:
            return;
    }
}

void
harpy::nest::windowing::input_controller::static_input_callback_manager::cursor_position_callback(GLFWwindow *window,
                                                                                                  double x, double y) {
    controller->cursor_position_action.invoke(&x, &y);
}

void harpy::nest::windowing::input_controller::static_input_callback_manager::mouse_button_callback(GLFWwindow *window,
                                                                                                    int button,
                                                                                                    int action,
                                                                                                    int mods) {
    button = (button | (mods << 10)) + 1;
    if(action)
        controller->mouse_map[button].invoke();
    else
        controller->mouse_map[-button].invoke();
}

void harpy::nest::windowing::input_controller::static_input_callback_manager::scroll_callback(GLFWwindow *window,
                                                                                              double xoffset,
                                                                                              double yoffset) {

}

void harpy::nest::windowing::input_controller::static_input_callback_manager::drop_smth_callback(GLFWwindow *window,
                                                                                                 int count,
                                                                                                 const char **paths) {

}

harpy::nest::windowing::input_controller::input_controller(harpy::nest::windowing::window *win) {
    this->win = win;
    glfwSetCursorPosCallback(this->win->get_glfw_window(), static_input_callback_manager::cursor_position_callback);
    glfwSetKeyCallback(this->win->get_glfw_window(), static_input_callback_manager::key_callback);
    glfwSetMouseButtonCallback(this->win->get_glfw_window(), static_input_callback_manager::mouse_button_callback);
}

void harpy::nest::windowing::input_controller::map_key(int key_code, harpy::nest::windowing::input_action action,
                                                       harpy::utilities::delegate delegate, int modifier) {
    set_callback_manager();
    key_code |= modifier << 10;
    switch(action){
        case input_action::press:{
            press_keymap[key_code] = std::move(delegate);
            break;
        }
        case input_action::repeat:{
            repeat_keymap[key_code] = std::move(delegate);
            break;
        }
        case input_action::release:{
            release_keymap[key_code] = std::move(delegate);
            break;
        }
    }
}

void harpy::nest::windowing::input_controller::unmap_key(int key_code, harpy::nest::windowing::input_action action,
                                                         int modifier) {
    set_callback_manager();
    key_code |= modifier << 10;
    switch(action){
        case input_action::press:{
            press_keymap.erase(key_code);
            break;
        }
        case input_action::repeat:{
            repeat_keymap.erase(key_code);
            break;
        }
        case input_action::release:{
            release_keymap.erase(key_code);
            break;
        }
    }

}

void harpy::nest::windowing::input_controller::update_key(int key_code, harpy::nest::windowing::input_action action,
                                                          harpy::utilities::delegate delegate, int modifier) {
    set_callback_manager();
    key_code |= modifier << 10;
    switch(action){
        case input_action::press:{
            press_keymap[key_code] = std::move(delegate);
            break;
        }
        case input_action::repeat:{
            repeat_keymap[key_code] = std::move(delegate);
            break;
        }
        case input_action::release:{
            release_keymap[key_code] = std::move(delegate);
            break;
        }
    }
}

void harpy::nest::windowing::input_controller::map_mouse_button(int button, utilities::delegate delegate, bool on_press,  int modifier) {
    set_callback_manager();
    button = (button | (modifier << 10)) + 1;
    mouse_map[on_press ? button : -button] = std::move(delegate);
}

void harpy::nest::windowing::input_controller::unmap_mouse_button(int button, bool on_press, int modifier) {
    set_callback_manager();
    button = (button | (modifier << 10)) + 1;
    mouse_map.erase(on_press ? button : -button);
}

void harpy::nest::windowing::input_controller::update_mouse_button(int button, harpy::utilities::delegate delegate,
                                                                   bool on_press, int modifier) {
    set_callback_manager();
    button = (button | (modifier << 10)) + 1;
    mouse_map[on_press ? button : -button] = std::move(delegate);
}

void harpy::nest::windowing::input_controller::disable_cursor() {
    glfwSetInputMode(win->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

std::pair<double, double> harpy::nest::windowing::input_controller::get_cursor_position() {
    set_callback_manager();
    std::pair<double, double> pos{};
    glfwGetCursorPos(win->get_glfw_window(), &pos.first, &pos.second);
    return pos;
}

void harpy::nest::windowing::input_controller::hide_cursor() {
    set_callback_manager();
    glfwSetInputMode(win->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void harpy::nest::windowing::input_controller::capture_cursor() {
    set_callback_manager();
    glfwSetInputMode(win->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
}

void harpy::nest::windowing::input_controller::normalize_cursor() {
    set_callback_manager();
    glfwSetInputMode(win->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void harpy::nest::windowing::input_controller::turn_raw_mouse_motion(bool is_raw) {
    set_callback_manager();
    if(glfwRawMouseMotionSupported() && glfwGetInputMode(win->get_glfw_window(), GLFW_CURSOR_DISABLED))
        glfwSetInputMode(win->get_glfw_window(), GLFW_RAW_MOUSE_MOTION, is_raw);
    else
        glfwSetInputMode(win->get_glfw_window(), GLFW_RAW_MOUSE_MOTION, false);
}

void harpy::nest::windowing::input_controller::add_cursor(std::string cursor_id, harpy::utilities::image *image,
                                                          int cursor_width, int cursor_height) {
    set_callback_manager();
    int format = image->get_current_format();
    if(image->get_channels() == 4)
        image->convert_image_color_space(cv::COLOR_BGRA2RGBA);
    else
        image->convert_image_color_space(cv::COLOR_BGR2RGBA);
    GLFWimage glfWimage{.width = cursor_width,
                        .height = cursor_height,
                        .pixels = image->get_cv_data().data};
    GLFWcursor* cursor = glfwCreateCursor(&glfWimage, 0 ,0);
    cursors[cursor_id] = cursor;

}

harpy::nest::windowing::input_controller::~input_controller() {
    for(auto& i : cursors){
        glfwDestroyCursor(i.second);
    }
}

void harpy::nest::windowing::input_controller::set_cursor(std::string id) {
    set_callback_manager();
    glfwSetCursor(win->get_glfw_window(), cursors[id]);
}

void harpy::nest::windowing::input_controller::destroy_cursor(std::string id) {
    glfwDestroyCursor(cursors[id]);
    cursors.erase(id);
}

void harpy::nest::windowing::input_controller::set_default_cursor() {
    glfwSetCursor(win->get_glfw_window(), nullptr);
}

void harpy::nest::windowing::input_controller::process_inputs() {
    glfwPollEvents();
}

void harpy::nest::windowing::input_controller::wait_for_inputs(float seconds) {
    if(seconds)
        glfwWaitEvents();
    else
        glfwWaitEventsTimeout(seconds);
}

void harpy::nest::windowing::input_controller::wake_from_waiting() {
    glfwPostEmptyEvent();
}

double harpy::nest::windowing::input_controller::get_time_elapsed_since_start() {
    return glfwGetTime();
}

void harpy::nest::windowing::input_controller::set_callback_manager() {
    static_input_callback_manager::set_instance(this);
}

void harpy::nest::windowing::input_controller::map_mouse_movement(
        harpy::utilities::parameters_delegate<double *, double *> action) {
    cursor_position_action = std::move(action);
}

void harpy::nest::windowing::input_controller::unmap_mouse_movement() {
    cursor_position_action.clear();
}

void harpy::nest::windowing::input_controller::update_mouse_movement(
        harpy::utilities::parameters_delegate<double *, double *> action) {
    cursor_position_action = std::move(action);
}





