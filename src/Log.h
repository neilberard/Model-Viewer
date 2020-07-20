#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

class Log {

public:
	static void Init();
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:

	static std::shared_ptr<spdlog::logger> s_ClientLogger;

};


#ifndef NDEBUG // DEBUG
#define LOG_ERROR(...) ::Log::GetClientLogger()->error(__VA_ARGS__)

#else
#define LOG_ERROR(...) // RELEASE
#endif // DEBUG


