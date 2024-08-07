//
// Created by Xyeveloper on 27.07.2024.
//

#ifndef HARPY_NEST_WINDOWING_INPUT_CONTROLLER
#define HARPY_NEST_WINDOWING_INPUT_CONTROLLER
#include <nest/resources/common_vulkan_resource.h>
#include <map>

namespace harpy::nest::windowing {

    enum class input_action{
        press,
        repeat,
        release,

        he_first = press,
        he_bm = 0,
        he_sum = 3
    };

    class input_controller {
        window* win{};
        std::unordered_map<std::string, GLFWcursor*> cursors{};

        std::map<std::pair<double, double>, utilities::delegate> cursor_position_map{};

        std::unordered_map<int, utilities::delegate> press_keymap{};
        std::unordered_map<int, utilities::delegate> repeat_keymap{};
        std::unordered_map<int, utilities::delegate> release_keymap{};
        std::unordered_map<int, utilities::delegate> mouse_map{};
        std::pair<bool, utilities::delegate> scroll_action{};
        utilities::delegate on_drop_action{};

        void set_callback_manager();


        class static_input_callback_manager{
            inline static input_controller* controller{};
        public:
            static void set_instance(input_controller* controller);
            static input_controller* get_input_controller();

            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

            //NOT IMPLEMENTED
            static void character_callback(GLFWwindow* window, uint32_t codepoint);
            static void cursor_enter_callback(GLFWwindow* window, int entered);


            static void cursor_position_callback(GLFWwindow* window, double x, double y);
            static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

            //X offset is not used, but being provided
            static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

            //Activates when a file/files/directories/directory is dropped on a window
            static void drop_smth_callback(GLFWwindow* window, int count, const char** paths);
        };


    public:
        input_controller(window* win = &resources::common_vulkan_resource::get_resource().get_main_window());

        void map_key(int key_code, input_action action, utilities::delegate delegate, int modifier = 0);
        void unmap_key(int key_code, input_action action, int modifier = 0);
        void update_key(int key_code, input_action action, utilities::delegate delegate, int modifier = 0);

        void map_mouse_button(int button, utilities::delegate delegate, bool on_press = true, int modifier = 0);
        void unmap_mouse_button(int button, bool on_press, int modifier = 0);
        void update_mouse_button(int button, utilities::delegate delegate, bool on_press, int modifier = 0);

        //SCROLLER NOT IMPLEMENTED
        void map_scroller();
        void unmap_scroller();
        void update_scroller();

        //JOYSTICK IS NON IMPLEMENTED
        void map_joystick();
        void unmap_joystick();
        void update_joystick();

        //GAMEPAD IS NON IMPLEMENTED
        void map_gamepad();
        void unmap_gamepad();
        void update_gamepad();

        void map_drop_action(utilities::delegate delegate);
        void unmap_drop_action();
        void update_drop_action(utilities::delegate delegate);

        uint32_t get_char();

        std::string get_key_name(int key_code, int scancode);
        int key_to_key_code(std::string key);

        std::pair<double, double> get_cursor_position();

        void disable_cursor();
        void hide_cursor();
        void capture_cursor();
        void normalize_cursor();

        /*When the cursor is disabled, raw (unscaled and unaccelerated) mouse motion can be enabled if available.
        Raw mouse motion is closer to the actual motion of the mouse across a surface. It is not affected by the scaling
         and acceleration applied to the motion of the desktop cursor. That processing is suitable for a cursor
         while raw motion is better for controlling for example a 3D camera. Because of this,
         raw mouse motion is only provided when the cursor is disabled.*/
        void turn_raw_mouse_motion(bool is_raw);

        void add_cursor(std::string cursor_id,
                        utilities::image* image = nullptr,
                        int cursor_width = 32,
                        int cursor_height = 32);

        void set_cursor(std::string id);
        void destroy_cursor(std::string id);
        void set_default_cursor();

        bool is_hovered_over_content_area();

        static void process_inputs();
        static void wait_for_inputs(float seconds = 0);
        static void wake_from_waiting();
        static double get_time_elapsed_since_start();

        ~input_controller();
    };
}

#endif //HARPY_NEST_WINDOWING_INPUT_CONTROLLER
