#pragma once

#if _DEBUG
#define HASSERT(condition)                                                                             \
    do                                                                                                 \
    {                                                                                                  \
        if (!(condition))                                                                              \
        {                                                                                              \
            std::cerr << "\033[31mAssertion failed: (" << #condition << "), function " << __FUNCTION__ \
                      << ", file " << __FILE__ << ", line " << __LINE__ << "\033[0m" << std::endl;     \
            std::abort();                                                                              \
        }                                                                                              \
    } while (false)
#define HASSERT_LOG(condition, log) HASSERT(condition)
#else
#define HASSERT(condition)                        \
    do                                            \
    {                                             \
        if (!(condition))                         \
        {                                         \
            throw std::runtime_error(#condition); \
        }                                         \
    } while (false)

#define HASSERT_LOG(condition, log)            \
    do                                     \
    {                                      \
        if (!(condition))                  \
        {                                  \
            throw std::runtime_error(log); \
        }                                  \
    } while (false)
#endif
