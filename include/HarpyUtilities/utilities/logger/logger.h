#pragma once
#ifndef HARPY_UTILITIES_LOGGER
#define HARPY_UTILITIES_LOGGER
#include <chrono>
#include <fstream>
#include <mutex>
#include <unordered_map>

#include "harpy_little_error.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace harpy::utilities
{
    
    //probably thread-safe singleton logger
    class HARPY_UTILITIES_API logger
    {
        static inline std::ofstream filelog{};
        static inline sz::string date{
            std::format(
            "{:%F}log.txt",
            std::chrono::zoned_time{std::chrono::current_zone(),
                std::chrono::system_clock::now()})
        };
        static inline std::mutex mutex{};
        //Эх, вот бы работало constinit
        static inline const  std::unordered_map<error_severity, sz::string> font_colors{
        {error_severity::just_notate_dude, "\033[37m"},
            {error_severity::message, "\033[96m"},
            {error_severity::warning, "\033[33m"},
            {error_severity::wrong_init, "\033[94m"},
            {error_severity::not_init, "\033[95m"},
            {error_severity::vulkan_error, "	\033[91m"},
            {error_severity::error, "\033[31m"},
                {error_severity::we_are_fucked, "\033[41m \033[97m"},
            {error_severity::harpys_eggs_under_attack, "\033[41m \033[97m"}
        };
        static inline const sz::string reset_color{"\033[0m"};

        sz::string tag{};
        sz::string last_log{};


        void private_log(error_severity severity, sz::string_view error);
        void private_log(harpy_little_error& error);

        void format_last_log(error_severity sever, sz::string_view error);
        void format_last_log(harpy_little_error& error);

        std::string get_severity_str(error_severity error_sever);

    public:
        logger(sz::string_view tag);

        void log(harpy_little_error& error);
        //use just for messaging, no need in shitposting in logs
        void log(error_severity severity, sz::string_view error);

        void cmd_log(harpy_little_error& error);
        void cmd_log(error_severity, sz::string_view error);

        void fast_cmd_log(harpy_little_error& error);
        void fast_cmd_log(error_severity, sz::string_view error);

        logger& operator<<(sz::string_view str);
        logger& operator<<(std::pair<error_severity, sz::string_view>);

        void show_last_log();
        static logger& get_logger();

    };
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //HARPY_UTILITIES_LOGGER
