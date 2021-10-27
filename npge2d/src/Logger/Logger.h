#ifndef LOGGER_H
#define LOGGER_H

#include "log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

#include <memory>

class Logger
{
private:
	
public:
	Logger() = default;
	~Logger() = default;

	void Initialize();
	void Destroy();
};

#endif // !LOGGER_H
