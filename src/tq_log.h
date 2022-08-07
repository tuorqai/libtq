//------------------------------------------------------------------------------
// Copyright (c) 2021-2022 tuorqai
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//------------------------------------------------------------------------------

#ifndef TQ_LOG_H_INC
#define TQ_LOG_H_INC

//------------------------------------------------------------------------------

enum
{
    LIBTQ_LOG_DEBUG = -1,
    LIBTQ_LOG_INFO = 0,
    LIBTQ_LOG_WARNING,
    LIBTQ_LOG_ERROR,
};

//------------------------------------------------------------------------------

void libtq_log(int level, char const *fmt, ...);

#if defined(NDEBUG)
    #define libtq_debug
#else
    #define libtq_debug(...) \
        libtq_log(LIBTQ_LOG_DEBUG, __VA_ARGS__)
#endif

//------------------------------------------------------------------------------
// Deprecated macros:

#define tq_log_debug(...)   libtq_debug(__VA_ARGS__)
#define tq_log_info(...)    libtq_log(LIBTQ_LOG_INFO, __VA_ARGS__)
#define tq_log_warning(...) libtq_log(LIBTQ_LOG_WARNING, __VA_ARGS__)
#define tq_log_error(...)   libtq_log(LIBTQ_LOG_ERROR, __VA_ARGS__)
#define log_debug(...)      libtq_debug(__VA_ARGS__)
#define log_info(...)       libtq_log(LIBTQ_LOG_INFO, __VA_ARGS__)
#define log_warning(...)    libtq_log(LIBTQ_LOG_WARNING, __VA_ARGS__)
#define log_error(...)      libtq_log(LIBTQ_LOG_ERROR, __VA_ARGS__)

//------------------------------------------------------------------------------

#endif // TQ_LOG_H_INC
