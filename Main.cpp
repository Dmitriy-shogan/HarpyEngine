#include <renderer/renderer.h>

using namespace harpy;
int main()
{
	init_glfw();
	try
	{
		system("glsl_to_spiv.bat");
		nest::change_projection(90);
		renderer render;
		render.main_loop();
		render.clean_up();
	}
	catch (utilities::harpy_little_error& exp)
	{
		exp.show();
		exp.log_this();
	}
	utilities::harpy_little_error logger(utilities::error_severity::message, "Everything went fine on first try");
	logger.show();
	logger.log_this();
}