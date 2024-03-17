#pragma once
#include "pch.h"
#define LOG_LEVEL_VERBOSE 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_ERROR 4

#if _DEBUG
#define CURRENT_LOG_LEVEL LOG_LEVEL_VERBOSE
#else
#define CURRENT_LOG_LEVEL LOG_LEVEL_ERROR
#endif

#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define GET_CURRENT_TIME_STR() ([]() -> std::string { \
    auto now = std::chrono::system_clock::now(); \
    auto now_c = std::chrono::system_clock::to_time_t(now); \
    char buffer[100]; \
    std::strftime(buffer, sizeof(buffer), LOG_TIME_FORMAT, std::localtime(&now_c)); \
    return buffer; })()

template <typename... Args>
inline void LogMessage(const std::string &level, const std::string &color, const std::string &msg, Args &&...args)
{
    std::cout << GET_CURRENT_TIME_STR() << color << " [" << level << "] "
              << "\033[0m";
    printf(msg.c_str(), std::forward<Args>(args)...);
}

#define HLOG_VERBOSE(msg, ...) LogMessage("VERBOSE", "\033[36m", msg, ##__VA_ARGS__)
#define HLOG_INFO(msg, ...) LogMessage("INFO", "\033[32m", msg, ##__VA_ARGS__)
#define HLOG_WARNING(msg, ...) LogMessage("WARNING", "\033[33m", msg, ##__VA_ARGS__)
#define HLOG_ERROR(msg, ...) LogMessage("ERROR", "\033[31m", msg, ##__VA_ARGS__)