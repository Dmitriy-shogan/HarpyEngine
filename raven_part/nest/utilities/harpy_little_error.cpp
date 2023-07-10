#include "harpy_little_error.h"

#include <chrono>
#include <format>

int harpy::utilities::harpy_little_error::log_number = 1;
std::string harpy::utilities::harpy_little_error::todays_date{std::format("{:%F}", std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()})};

harpy::utilities::harpy_little_error::harpy_little_error(std::string&& error): runtime_error(error), severity(error_severity::error)
{}

harpy::utilities::harpy_little_error::harpy_little_error(const std::string& error): runtime_error(error), severity(error_severity::error)
{}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity, std::string&& error): runtime_error(error), severity(severity)
{}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity, const std::string& error): runtime_error(error), severity(severity)
{}

void harpy::utilities::harpy_little_error::log_this()
{
    using namespace std;
    ofstream log(todays_date + " log№" + to_string(log_number) + ".txt", ios_base::app);
    if(!log.is_open())
    {
        ofstream create_file(todays_date + " log.txt");
        log.open(todays_date + " log.txt");
        create_file.close();
    }
    string sever;
    switch (INTCAST(severity))
    {

    case INTCAST(error_severity::not_init):
        {
            sever = "Not initialised";
            break;
        }
    case INTCAST(error_severity::wrong_init):
        {
            sever = "Wrong initialised";
            break;
        }
    case INTCAST(error_severity::harpys_eggs_under_attack):
        {
            sever = "HOLY MOLY TERRIBLE AS FUCK";
            break;
        }
    case INTCAST(error_severity::just_notate_dude):
        {
            sever = "Notate, man, chill";
            break;
        }
    case INTCAST(error_severity::message):
        {
            sever = "Message";
            break;
        }
    case INTCAST(error_severity::warning):
        {
            sever = "Warning";
            break;
        }
    case INTCAST(error_severity::error):
    default:
        {
            sever = "Error";
        }
    }
    log << "[" + format("{:%T}", chrono::zoned_time{chrono::current_zone(), chrono::system_clock::now()}) + "] " << "{" + sever + "}: " << what() <<endl;
    log.close();
}

void harpy::utilities::harpy_little_error::show()
{
    std::string sever;
    switch (INTCAST(severity))
    {
            
    case INTCAST(error_severity::harpys_eggs_under_attack):
        {
            sever = "HOLY MOLY TERRIBLE AS FUCK";
            break;
        }
    case INTCAST(error_severity::just_notate_dude):
        {
            sever = "Notate, man, chill";
            break;
        }
    case INTCAST(error_severity::message):
        {
            sever = "Message";
            break;
        }
    case INTCAST(error_severity::warning):
        {
            sever = "Warning";
            break;
        }
    case INTCAST(error_severity::error):
    default:
        {
            sever = "Error";
        }
    }
    std::cerr << "[" + todays_date + "  " +std::format("{:%T}", std::chrono::zoned_time{std::chrono::current_zone(), std::chrono::system_clock::now()}) + "] " << "{" + sever + "}: " << what() <<std::endl;
}
