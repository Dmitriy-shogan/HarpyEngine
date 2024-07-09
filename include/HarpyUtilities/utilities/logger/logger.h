#pragma once
#ifndef HARPY_UTILITIES_LOGGER
#define HARPY_UTILITIES_LOGGER
#include <chrono>
#include <fstream>
#include <mutex>
#include "harpy_little_error.h"

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4251)
#endif

namespace harpy::utilities::error_handling
{
    
    //probably thread-safe singleton logger
    class HARPY_UTILITIES_API logger
    {
        std::ofstream filelog{};
        std::mutex mutex{};
        std::string date{
            std::format(
            "{:%F}log.txt",
            std::chrono::zoned_time{std::chrono::current_zone(),
                std::chrono::system_clock::now()})
        };
        std::string last_log{};

        void private_log(error_severity severity, std::string error);
        void format_last_log(error_severity sever, std::string error);
        std::string get_severity_str(error_severity error_sever);

        logger() = default;
        
    public:
        
        logger(logger const &) = delete;
        logger(logger const &&) = delete;

        logger(logger &) = delete;
        logger(logger &&) = delete;

        logger& operator =(logger const &) = delete;
        logger& operator =(logger const &&) = delete;
        
        logger& operator =(logger &) = delete;
        logger& operator =(logger &&) = delete;

        void log(harpy_little_error& error);

        //use just for messaging, no need in shitposting in logs
        void log(error_severity severity, std::string error);
        logger& operator<<(std::string str);
        logger& operator<<(std::pair<error_severity, std::string>);

        void show_last_log();

        static logger& get_logger();
        ~logger();
    };
}

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //HARPY_UTILITIES_LOGGER
