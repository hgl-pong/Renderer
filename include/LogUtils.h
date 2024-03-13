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

#define HLOG_VERBOSE(msg)                       \
    if (CURRENT_LOG_LEVEL <= LOG_LEVEL_VERBOSE) \
        std::cout << GET_CURRENT_TIME_STR() << "\033[36m [VERBOSE] \033[0m" << msg << std::endl;
#define HLOG_INFO(msg)                       \
    if (CURRENT_LOG_LEVEL <= LOG_LEVEL_INFO) \
        std::cout << GET_CURRENT_TIME_STR() << "\033[32m [INFO] \033[0m" << msg << std::endl;
#define HLOG_WARNING(msg)                       \
    if (CURRENT_LOG_LEVEL <= LOG_LEVEL_WARNING) \
        std::cout << GET_CURRENT_TIME_STR() << "\033[33m [WARNING] \033[0m" << msg << std::endl;
#define HLOG_ERROR(msg)                      \
    if (CURRENT_LOG_LEVEL <= LOG_LEVEL_ERROR) \
        std::cerr << GET_CURRENT_TIME_STR() << "\033[31m [ERROR] \033[0m" << msg << std::endl;
