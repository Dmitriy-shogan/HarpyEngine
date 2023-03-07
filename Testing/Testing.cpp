#include "utilities/harpy_little_error.h"
int main(int argc, char* argv[])
{
    harpy_nest::harpy_little_error error(harpy_nest::error_severity::just_notate_dude, "Chill, everything is working");
    error.log_this();
    error.show();
    return 0;
}
