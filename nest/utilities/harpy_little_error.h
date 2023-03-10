#pragma once
#ifndef HARPY_ERRORS
#define HARPY_ERRORS
#include "..//utilities/initializations.hpp"

namespace harpy_nest{

    enum class error_severity
    {
        just_notate_dude = -1,
        message,
        warning,
        not_init,
        wrong_init,
        error,
        we_are_fucked,
        harpys_eggs_under_attack = we_are_fucked,
     };
    
    class harpy_little_error : public std::runtime_error
    {
        static int log_number;
        static std::string todays_date;
        error_severity severity;
        
    public:
        harpy_little_error() = delete;
        harpy_little_error(std::string&& error);

        harpy_little_error(const std::string& error);

        harpy_little_error(error_severity severity, std::string&& error);

        harpy_little_error(error_severity severity, const std::string& error);

        void log_this();
        void show();
        
    };

}


#endif //HARPY_ERRORS