#include "rrpch.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#define RR_HAS_CONSOLE !RR_DIST
namespace Rose
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		std::vector<spdlog::sink_ptr> roseSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/ROSE.log", true),
#if RR_HAS_CONSOLE
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
		};

		std::vector<spdlog::sink_ptr> appSinks =
		{
			std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/APP.log", true),
#if RR_HAS_CONSOLE
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
		};


		roseSinks[0]->set_pattern("[%T] [%l] %n: %v");
		appSinks[0]->set_pattern("[%T] [%l] %n: %v");

#if RR_HAS_CONSOLE
		roseSinks[1]->set_pattern("%^[%T] %n: %v%$");
		appSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

		s_CoreLogger = std::make_shared<spdlog::logger>("ROSE", begin(roseSinks), end(roseSinks));
		s_CoreLogger->set_level(spdlog::level::trace);
		
		s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(appSinks), end(appSinks));
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}