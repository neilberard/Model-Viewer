#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

class Log {

public:
	static void Init();	
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	static void set_level();
private:

	static std::shared_ptr<spdlog::logger> s_ClientLogger;

};


#ifndef NDEBUG // DEBUG
#define SET_LOG_LEVEL_DEBUG 
#define LOG_CRITICAL(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->error(__VA_ARGS__); __debugbreak()
#define LOG_NOT_IMPLEMENTED_ERROR(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->error(__VA_ARGS__); __debugbreak()
#define LOG_WARNING(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_DEBUG(...) if(Log::GetClientLogger() != NULL) ::Log::GetClientLogger()->debug(__VA_ARGS__)

#else
#define LOG_CRITICAL(...) // RELEASE
#define LOG_ERROR(...)
#define LOG_WARNING(...)
#define LOG_INFO(...)
#define LOG_DEBUG(...)


#endif // DEBUG


