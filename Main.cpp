#include "harpy.h"

int main()
{



	try
	{
		harpy::init();
		harpy::run();
	}
	catch (harpy::utilities::harpy_little_error& exp)
	{
		exp.show();
		exp.log_this();
	}
	harpy::utilities::harpy_little_error logger(harpy::utilities::error_severity::message, "Everything went fine on first try");
	logger.show();
	logger.log_this();
}
