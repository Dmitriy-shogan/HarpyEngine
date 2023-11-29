#include "logger.h"

#include <iostream>

using namespace harpy::utilities::error_handling;

void logger::private_log(error_severity severity, std::string error)
{
    if(filelog.bad())
        filelog.clear();
    
    if (!filelog.is_open())
    {
        filelog.open(date, std::ios_base::app);
        if (!filelog.is_open())
        {
            std::ofstream create_file(date);
            filelog.open(date);
            create_file.close();
        }
    }
    format_last_log(severity, error);
    filelog << last_log << std::endl;
}

void logger::format_last_log(error_severity sever, std::string error)
{
    last_log = std::format("[{}]({}):{}",
        std::format("{:%T}", std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()})
        , get_severity_str(sever)
        , error
        );
}

std::string logger::get_severity_str(error_severity error_sever)
{
    switch (static_cast<int>(error_sever))
    {
    case static_cast<int>(error_severity::not_init):
        return "Not initialised";
            
    case static_cast<int>(error_severity::wrong_init):
        return "Wrong initialised";
            
    case static_cast<int>(error_severity::harpys_eggs_under_attack):
        return "HOLY MOLY TERRIBLE AS FUCK";
            
    case static_cast<int>(error_severity::just_notate_dude):
        return "Notate, man, chill";
            
    case static_cast<int>(error_severity::message):
        return "Message";
            
    case static_cast<int>(error_severity::warning):
        return "Warning";
            
    case static_cast<int>(error_severity::error):
    default:
        return "Error";
    }
}

void logger::log(harpy_little_error& error)
{
    std::scoped_lock lock{mutex};
    
    if(filelog.bad())
        filelog.clear();
    
    if (!filelog.is_open())
    {
        filelog.open(date, std::ios_base::app);
        if (!filelog.is_open())
        {
            std::ofstream create_file(date);
            filelog.open(date);
            create_file.close();
        }
    }
    format_last_log(error.severity, error.what());
    filelog << last_log << std::endl;
}

void logger::log(error_severity severity, std::string error)
{
    std::scoped_lock lock{mutex};

    //I don't know why, but static ofstream starts with bad = true
    if(filelog.bad())
        filelog.clear();
    
    if (!filelog.is_open())
    {
        filelog.open(date, std::ios_base::app);
        if (!filelog.is_open())
        {
            std::ofstream create_file(date);
            filelog.open(date);
            create_file.close();
        }
    }
    format_last_log(severity, error);
    filelog << last_log << std::endl;
}

logger& logger::operator<<(std::string str)
{
    std::scoped_lock lock{mutex};
    private_log(error_severity::message, str);
    std::clog << last_log << std::endl;
    return *this;
}

logger& logger::operator<<(std::pair<error_severity, std::string> pair)
{
    std::scoped_lock lock{mutex};
    private_log(pair.first, pair.second);
    std::clog << last_log << std::endl;
    return *this;
}

void logger::show_last_log()
{
    std::scoped_lock lock{mutex};
    std::cerr << last_log << std::endl;
}

logger& logger::get_logger()
{
    static logger love_to_log{};
    return love_to_log;
}

logger::~logger()
{
    filelog << "SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR_SEPARATOR";
    filelog.close();
}
