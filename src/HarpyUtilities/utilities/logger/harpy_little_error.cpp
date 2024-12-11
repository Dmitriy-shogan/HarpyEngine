#include <logger/harpy_little_error.h>

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity,
    sz::string_view const&& error) : runtime_error(error), severity(severity)
{
}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity, sz::string_view&& error)
: runtime_error(error), severity(severity)
{
}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity,
    sz::string_view const& error) : runtime_error(error), severity(severity)
{
}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity, sz::string_view& error)
: runtime_error(error), severity(severity)
{
}

harpy::utilities::harpy_little_error::harpy_little_error(error_severity severity, const char* error)
    :runtime_error(error), severity(severity)
{
}


harpy::utilities::harpy_little_error::harpy_little_error(sz::string_view error) :runtime_error(error), severity(error_severity::error)
{
}
