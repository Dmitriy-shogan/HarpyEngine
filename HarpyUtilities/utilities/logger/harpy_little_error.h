#pragma once
#ifndef HARPY_UTILITIES_LITTLE_ERROR
#define HARPY_UTILITIES_LITTLE_ERROR
#include <stdexcept>

namespace harpy::utilities::error_handling
{
    
    enum class error_severity
    {
        he_first = -1,
        
        just_notate_dude = -1,
        message,
        warning,
        not_init,
        wrong_init,
        error,
        we_are_fucked,
        harpys_eggs_under_attack = we_are_fucked,

        he_is_bm = 0,
        he_count = 8
     };

    
    class harpy_little_error : public std::runtime_error
    {
    public:

        harpy_little_error() = delete;

        harpy_little_error(error_severity severity, std::string const && error);
        harpy_little_error(error_severity severity, std::string&& error);

        harpy_little_error(error_severity severity, std::string const & error);
        harpy_little_error(error_severity severity, std::string& error);

        harpy_little_error(error_severity severity, const char* error);
        harpy_little_error(std::string error);

        

        const error_severity severity;
    
    };
}



#endif //HARPY_UTILITIES_LITTLE_ERROR