#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Rose 
{
	class Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

//Core log macros
#define RR_CORE_FATAL(...) ::Rose::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define RR_CORE_ERROR(...) ::Rose::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RR_CORE_WARN(...)  ::Rose::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RR_CORE_INFO(...)  ::Rose::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RR_CORE_TRACE(...) ::Rose::Log::GetCoreLogger()->trace(__VA_ARGS__)

//Cleint log macros
#define RR_FATAL(...)      ::Rose::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define RR_ERROR(...)      ::Rose::Log::GetClientLogger()->error(__VA_ARGS__)
#define RR_WARN(...)       ::Rose::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RR_INFO(...)       ::Rose::Log::GetClientLogger()->info(__VA_ARGS__)
#define RR_TRACE(...)      ::Rose::Log::GetClientLogger()->trace(__VA_ARGS__)