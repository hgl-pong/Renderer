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
#define HASSERT(condition)                                                      \
    do                                                                          \
    {                                                                           \
        if (!(condition))                                                       \
        {                                                                       \
            HLOG_ERROR("Assertion failed: (%s), function %s, file %s, line %d", \
                       #condition, __FUNCTION__, __FILE__, __LINE__);           \
            std::abort();                                                       \
        }                                                                       \
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
#define VK_CHECK_RESULT(f)                                                                                  \
    {                                                                                                       \
        VkResult res = (f);                                                                                 \
        if (res != VK_SUCCESS)                                                                              \
        {                                                                                                   \
            std::cerr << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ \
                      << std::endl;                                                                         \
            std::abort();                                                                                   \
        }                                                                                                   \
    }
#define MODULE_TEST 1