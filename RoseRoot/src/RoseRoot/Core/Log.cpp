#include "rrpch.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
namespace Rose
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");

		auto  sharedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("log.log");

		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
		sinks.push_back(sharedFileSink);

		s_CoreLogger = std::make_shared<spdlog::logger>("ROSE", begin(sinks), end(sinks));
		s_CoreLogger->set_level(spdlog::level::trace);
		
		s_ClientLogger = std::make_shared<spdlog::logger>("APP ", begin(sinks), end(sinks));
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}