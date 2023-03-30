#include <renderer/renderer.h>

using namespace harpy_nest;
//TODO: in all available hpp files rewrite includes to be in <> brackets instead of direct way
int main()
{
	init_glfw();
	try
	{
		system("glsl_to_spiv.bat");
		harpy_renderer::renderer render;
		render.main_loop();
		render.clean_up();
	}
	catch (harpy_little_error& exp)
	{
		exp.show();
		exp.log_this();
	}
	harpy_little_error logger(error_severity::message, "Everything went fine on first try");
	logger.show();
	logger.log_this();
}