#pragma once
#include "pch.h"

enum LogLevel
{
    LOG_LEVEL_VERBOSE = 1,
    LOG_LEVEL_INFO = 2,
    LOG_LEVEL_WARNING = 3,
    LOG_LEVEL_ERROR = 4
};

#ifndef DISABLE_HLOG
#if _DEBUG
#define CURRENT_LOG_LEVEL LOG_LEVEL_VERBOSE
#else
#define CURRENT_LOG_LEVEL LOG_LEVEL_ERROR
#endif //!_DEBUG
#else
#define CURRENT_LOG_LEVEL (LOG_LEVEL_ERROR + 1)
#endif // !DISABLE_HLOG

#define LOG_TIME_FORMAT "%Y-%m-%d %H:%M:%S"
#define GET_CURRENT_TIME_STR() ([]() -> std::string { \
    auto now = std::chrono::system_clock::now(); \
    auto now_c = std::chrono::system_clock::to_time_t(now); \
    char buffer[100]; \
    std::strftime(buffer, sizeof(buffer), LOG_TIME_FORMAT, std::localtime(&now_c)); \
    return buffer; })()

static std::mutex logMutex;

inline std::string GetLogColor(const int &level)
{
    if (level == LOG_LEVEL_VERBOSE)
        return "\033[36m";
    else if (level == LOG_LEVEL_INFO)
        return "\033[32m";
    else if (level == LOG_LEVEL_WARNING)
        return "\033[33m";
    else if (level == LOG_LEVEL_ERROR)
        return "\033[31m";
    else
        return "\033[0m";
}

inline std::string GetLogLevelStr(const int &level)
{
    if (level == LOG_LEVEL_VERBOSE)
        return "VERBOSE";
    else if (level == LOG_LEVEL_INFO)
        return "INFO";
    else if (level == LOG_LEVEL_WARNING)
        return "WARNING";
    else if (level == LOG_LEVEL_ERROR)
        return "ERROR";
    else
        return "UNKNOWN";
}

template <typename... Args>
inline void LogMessage(const int &level, const std::string &msg, Args &&...args)
{
    if (level < CURRENT_LOG_LEVEL)
        return;
    std::lock_guard<std::mutex> lock(logMutex);
    std::string timeStr = GET_CURRENT_TIME_STR();
    std::string printLogStr = timeStr + GetLogColor(level) + " [" + GetLogLevelStr(level) + "] " + "\033[0m" + msg + "\n";
    printf(printLogStr.c_str(), std::forward<Args>(args)...);
}

#define HLOG(level, msg, ...)                  \
    do                                         \
    {                                          \
        LogMessage(level, msg, ##__VA_ARGS__); \
    } while (false)

#define HLOG_VERBOSE(msg, ...) HLOG(LOG_LEVEL_VERBOSE, msg, ##__VA_ARGS__)
#define HLOG_INFO(msg, ...) HLOG(LOG_LEVEL_INFO, msg, ##__VA_ARGS__)
#define HLOG_WARNING(msg, ...) HLOG(LOG_LEVEL_WARNING, msg, ##__VA_ARGS__)
#define HLOG_ERROR(msg, ...) HLOG(LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)


#define MATHLOG(level, msg, ...) HLOG(level, msg, ...)
#define MATHLOG_VERBOSE(msg, ...) HLOG_VERBOSE(msg, ...)
#define MATHLOG_INFO(msg, ...)    HLOG_INFO(msg, ...)
#define MATHLOG_WARNING(msg, ...) HLOG_WARNING(msg, ...)
#define MATHLOG_ERROR(msg, ...)   HLOG_ERROR(msg, ...) 