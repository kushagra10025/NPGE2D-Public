#ifndef LOG_H
#define LOG_H

#include <spdlog/spdlog.h>

const std::string NPGE_DEFAULT_LOGGER_NAME = "NPGE";

// Add Assertions in the Logger

#ifndef NPGE_CONFIG_RELEASE
#define NPGE_TRACE(...) 			if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->trace(__VA_ARGS__);}
#define NPGE_DEBUG(...) 			if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->debug(__VA_ARGS__);}
#define NPGE_INFO(...) 				if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->info(__VA_ARGS__);}
#define NPGE_WARN(...) 				if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->warn(__VA_ARGS__);}
#define NPGE_ERROR(...) 			if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->error(__VA_ARGS__);}
#define NPGE_CRITICAL(...) 			if (spdlog::get(NPGE_DEFAULT_LOGGER_NAME) != nullptr) {spdlog::get(NPGE_DEFAULT_LOGGER_NAME)->critical(__VA_ARGS__);}
#else
#define NPGE_TRACE(...)				(void)0
#define NPGE_DEBUG(...)				(void)0
#define NPGE_INFO(...)				(void)0
#define NPGE_WARN(...)				(void)0
#define NPGE_ERROR(...)				(void)0
#define NPGE_CRITICAL(...)			(void)0
#endif

#endif