#include "Logger.h"

void Logger::Initialize()
{
	std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	//consoleSink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] %v%$");
	consoleSink->set_pattern("%^[%T] %n: %v%$");


	std::vector<spdlog::sink_ptr> sinks{ consoleSink };

	std::shared_ptr<spdlog::logger> logger = std::make_shared<spdlog::logger>(NPGE_DEFAULT_LOGGER_NAME, sinks.begin(), sinks.end());
	logger->set_level(spdlog::level::trace);
	logger->flush_on(spdlog::level::trace);

	spdlog::register_logger(logger);
}

void Logger::Destroy()
{
	spdlog::shutdown();
}
