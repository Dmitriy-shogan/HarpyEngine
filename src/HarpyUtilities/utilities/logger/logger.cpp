#include <logger/logger.h>

#include <iostream>

using namespace harpy::utilities;

void logger::private_log(error_severity severity, sz::string_view error)
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

void logger::private_log(harpy_little_error &error) {
    private_log(error.severity, error.what());
}

void logger::format_last_log(error_severity sever, sz::string_view error)
{
    last_log = std::format("[{}]({})<{}>: {}",
    std::format("{:%H:%M:%S}", std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::current_zone()
    ->to_local(std::chrono::system_clock::now())))
        , get_severity_str(sever)
        , std::string{tag} //TODO: make fmt support sz::string
        , std::string{error}
        );
}

void logger::format_last_log(harpy_little_error &error) {
    format_last_log(error.severity, error.what());
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
        return "Notation";

    case static_cast<int>(error_severity::message):
        return "Message";

    case static_cast<int>(error_severity::warning):
        return "Warning";

    case static_cast<int>(error_severity::error):
    default:
        return "Error";
    }
}

logger::logger(sz::string_view tag): tag(tag) {
}

void logger::log(harpy_little_error& error)
{
    std::scoped_lock lock{mutex};
    private_log(error);
}

void logger::log(error_severity severity, sz::string_view error)
{
    std::scoped_lock lock{mutex};
    private_log(severity, error);
}

void logger::cmd_log(harpy_little_error &error) {
   cmd_log(error.severity, error.what());
}

void logger::cmd_log(error_severity sever, sz::string_view error) {
    std::scoped_lock lock{mutex};
    fast_cmd_log(sever, error);
}

void logger::fast_cmd_log(harpy_little_error &error) {
    fast_cmd_log(error.severity, error.what());
}

void logger::fast_cmd_log(error_severity sever, sz::string_view error) {
    format_last_log(sever, error);
    colored_log = last_log;
    colored_log.insert(colored_log.find_first_of(">")+2, font_colors.at(sever));
    colored_log.append(reset_color);
    std::cout << colored_log << std::endl;
}

logger& logger::operator<<(sz::string_view str)
{
    std::scoped_lock lock{mutex};
    private_log(error_severity::message, str);
    fast_cmd_log(error_severity::message, str);
    return *this;
}

logger& logger::operator<<(std::pair<error_severity, sz::string_view> pair)
{
    std::scoped_lock lock{mutex};
    private_log(pair.first, pair.second);
    fast_cmd_log(pair.first, pair.second);
    return *this;
}

void logger::show_last_log()
{
    std::scoped_lock lock{mutex};
    fast_cmd_log(error_severity::message, last_log);
}

logger& logger::get_logger() {
    static logger static_logger{"Main"};
    return static_logger;
}
