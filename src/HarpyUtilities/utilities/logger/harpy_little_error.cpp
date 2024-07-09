#include <logger/harpy_little_error.h>

harpy::utilities::error_handling::harpy_little_error::harpy_little_error(error_severity severity,
    std::string const&& error) : runtime_error(error), severity(severity)
{
}

harpy::utilities::error_handling::harpy_little_error::harpy_little_error(error_severity severity, std::string&& error)
: runtime_error(error), severity(severity)
{
}

harpy::utilities::error_handling::harpy_little_error::harpy_little_error(error_severity severity,
    std::string const& error) : runtime_error(error), severity(severity)
{
}

harpy::utilities::error_handling::harpy_little_error::harpy_little_error(error_severity severity, std::string& error)
: runtime_error(error), severity(severity)
{
}

harpy::utilities::error_handling::harpy_little_error::harpy_little_error(error_severity severity, const char* error)
    :runtime_error(error), severity(severity)
{
}


harpy::utilities::error_handling::harpy_little_error::harpy_little_error(std::string error) :runtime_error(error), severity(error_severity::error)
{
}
