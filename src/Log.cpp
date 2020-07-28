#include "Log.h"

std::shared_ptr<spdlog::logger> Log::s_ClientLogger;


void Log::Init()
{

	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_ClientLogger = spdlog::stdout_color_mt("APP");
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);
}

void Log::set_level()
{
	s_ClientLogger->set_level(spdlog::level::critical);

}
