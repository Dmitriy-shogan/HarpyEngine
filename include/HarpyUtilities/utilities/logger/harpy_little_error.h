#pragma once
#ifndef HARPY_UTILITIES_LITTLE_ERROR
#define HARPY_UTILITIES_LITTLE_ERROR
#include <stdexcept>
#include <stringzilla/stringzilla.hpp>
#include <../utilities_dll_macro.h>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4275)
#endif


namespace harpy::utilities
{
    
    enum class error_severity
    {
        just_notate_dude = -1,
        message,
        warning,
        not_init,
        wrong_init,
        vulkan_error,
        error,
        we_are_fucked,
        harpys_eggs_under_attack = we_are_fucked,
     };

    //This will produce a number of errors, but ms documentation says that if this is std, it's fine
    class HARPY_UTILITIES_API harpy_little_error : public std::runtime_error
    {
    public:

        harpy_little_error() = delete;

        harpy_little_error(error_severity severity, sz::string_view const && error);
        harpy_little_error(error_severity severity, sz::string_view&& error);

        harpy_little_error(error_severity severity, sz::string_view const & error);
        harpy_little_error(error_severity severity, sz::string_view& error);

        harpy_little_error(error_severity severity, const char* error);
        harpy_little_error(sz::string_view error);
        

        const error_severity severity;
    
    };
}


#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif //HARPY_UTILITIES_LITTLE_ERROR