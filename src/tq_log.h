
#ifndef TQ_LOG_H_INC
#define TQ_LOG_H_INC

//------------------------------------------------------------------------------

enum tq_log_level {
    TQ_LOG_LEVEL_DEBUG,
    TQ_LOG_LEVEL_INFO,
    TQ_LOG_LEVEL_WARNING,
    TQ_LOG_LEVEL_ERROR,
};

//------------------------------------------------------------------------------

void tq_log_generic(int level, char const *fmt, ...);

#if defined(NDEBUG)
#define tq_log_debug
#else
#define tq_log_debug(...)   tq_log_generic(TQ_LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#define tq_log_info(...)    tq_log_generic(TQ_LOG_LEVEL_INFO, __VA_ARGS__)
#define tq_log_warning(...) tq_log_generic(TQ_LOG_LEVEL_WARNING, __VA_ARGS__)
#define tq_log_error(...)   tq_log_generic(TQ_LOG_LEVEL_ERROR, __VA_ARGS__)

//------------------------------------------------------------------------------
// Deprecated macros:

#if defined(NDEBUG)
#define log_debug
#else
#define log_debug(...)      tq_log_generic(TQ_LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#define log_info(...)       tq_log_generic(TQ_LOG_LEVEL_INFO, __VA_ARGS__)
#define log_warning(...)    tq_log_generic(TQ_LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_error(...)      tq_log_generic(TQ_LOG_LEVEL_ERROR, __VA_ARGS__)

//------------------------------------------------------------------------------

#endif // TQ_LOG_H_INC
