
//------------------------------------------------------------------------------

#if defined(unix)

//------------------------------------------------------------------------------

#if defined(__linux__)
    #define _GNU_SOURCE
#endif

#include <errno.h>
#include <pthread.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------

struct thread_info
{
    pthread_t   thread;
    char const  *name;
    int         (*func)(void *);
    void        *data;
};

//------------------------------------------------------------------------------

static void *thread_main(void *arg)
{
    struct thread_info *info = (struct thread_info *) arg;
    int retval = info->func(info->data);

    tq_mem_free(info);

    return (void *) ((intptr_t) retval);
}

//------------------------------------------------------------------------------

static void initialize(void)
{
}

static void terminate(void)
{
}

static void sleep(double seconds)
{
    uint64_t s = (uint64_t) floor(seconds);

    struct timespec ts = {
        .tv_sec = s,
        .tv_nsec = (uint64_t) ((seconds - s) * 1000000000)
    };

    while ((nanosleep(&ts, &ts) == -1) && (errno == EINTR)) {}
}

static tq_thread_t create_thread(char const *name, int (*func)(void *), void *data)
{
    struct thread_info *info = tq_mem_alloc(sizeof(struct thread_info));
    
    info->name = name;
    info->func = func;
    info->data = data;

    int status = pthread_create(&info->thread, NULL, thread_main, info);

    if (status != 0) {
        tq_log_error("Error occured while attempting to create thread.\n");
        tq_mem_free(info);

        return NULL;
    }

    #if defined(__linux__)
        pthread_setname_np(info->thread, name);
    #endif

    return (tq_thread_t) info;
}

static void detach_thread(tq_thread_t thread)
{
    struct thread_info *info = (struct thread_info *) thread;
    int status = pthread_detach(info->thread);

    if (status != 0) {
        tq_log_error("Failed to detach thread \"%s\".\n", info->name);
    }
}

static int wait_thread(tq_thread_t thread)
{
    struct thread_info *info = (struct thread_info *) thread;

    void *retval;
    int status = pthread_join(info->thread, &retval);

    if (status != 0) {
        tq_log_error("Failed to join thread \"%s\".\n", info->name);
    }

    return (int) ((intptr_t) retval);
}

static tq_mutex_t create_mutex(void)
{
    pthread_mutex_t *mutex = tq_mem_alloc(sizeof(pthread_mutex_t));
    int status = pthread_mutex_init(mutex, NULL);

    if (status != 0) {
        tq_error("Failed to create mutex, error code: %d", status);
    }

    return (tq_mutex_t) mutex;
}

static void destroy_mutex(tq_mutex_t mutex)
{
    int status = pthread_mutex_destroy((pthread_mutex_t *) mutex);

    if (status != 0) {
        tq_log_error("Failed to destroy mutex, error code: %d", status);
    }

    tq_mem_free(mutex);
}

static void lock_mutex(tq_mutex_t mutex)
{
    pthread_mutex_lock((pthread_mutex_t *) mutex);
}

static void unlock_mutex(tq_mutex_t mutex)
{
    pthread_mutex_unlock((pthread_mutex_t *) mutex);
}

//------------------------------------------------------------------------------

void tq_construct_posix_threads(tq_threads_impl_t *impl)
{
    impl->initialize        = initialize;
    impl->terminate         = terminate;
    impl->sleep             = sleep;

    impl->create_thread     = create_thread;
    impl->detach_thread     = detach_thread;
    impl->wait_thread       = wait_thread;

    impl->create_mutex      = create_mutex;
    impl->destroy_mutex     = destroy_mutex;
    impl->lock_mutex        = lock_mutex;
    impl->unlock_mutex      = unlock_mutex;
}

//------------------------------------------------------------------------------

#endif // defined(unix)

//------------------------------------------------------------------------------
