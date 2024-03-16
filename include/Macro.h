#pragma once
#if defined(_WIN32) || defined(__WIN32__)
#ifdef HLIB_IMPORTS
#define HAPI __declspec(dllimport)
#else
#define HAPI __declspec(dllexport)
#endif
#else
#define HAPI
#endif
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

#define HASSERT_LOG(condition, log)        \
    do                                     \
    {                                      \
        if (!(condition))                  \
        {                                  \
            throw std::runtime_error(log); \
        }                                  \
    } while (false)
#endif
#define VK_RELEASE(object, deleter, device) \
    if (object != VK_NULL_HANDLE)           \
    {                                       \
        deleter(device, object, nullptr);   \
        object = VK_NULL_HANDLE;            \
    }