
#ifndef TQ_LOG_H_INC
#define TQ_LOG_H_INC

//------------------------------------------------------------------------------

enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
};

void log_generic(int level, char const *fmt, ...);

#if defined(NDEBUG)
#define log_debug
#else
#define log_debug(...)      log_generic(LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#define log_info(...)       log_generic(LOG_LEVEL_INFO, __VA_ARGS__)
#define log_warning(...)    log_generic(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_error(...)      log_generic(LOG_LEVEL_ERROR, __VA_ARGS__)

//------------------------------------------------------------------------------

#endif // TQ_LOG_H_INC
