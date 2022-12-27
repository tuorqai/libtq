
//------------------------------------------------------------------------------

#if defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)

//------------------------------------------------------------------------------

#define _GNU_SOURCE

#include <errno.h>
#include <pthread.h>

#include "tq_core.h"
#include "tq_error.h"
#include "tq_log.h"

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

    free(info);

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

static libtq_thread create_thread(char const *name, int (*func)(void *), void *data)
{
    struct thread_info *info = malloc(sizeof(struct thread_info));
    
    if (!info) {
        libtq_out_of_memory();
    }

    info->name = name;
    info->func = func;
    info->data = data;

    int status = pthread_create(&info->thread, NULL, thread_main, info);

    if (status != 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Error occured while attempting to create thread.\n");
        free(info);

        return NULL;
    }

    #if defined(__linux__)
        pthread_setname_np(info->thread, name);
    #endif

    return (libtq_thread) info;
}

static void detach_thread(libtq_thread thread)
{
    struct thread_info *info = (struct thread_info *) thread;
    int status = pthread_detach(info->thread);

    if (status != 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Failed to detach thread \"%s\".\n", info->name);
    }
}

static int wait_thread(libtq_thread thread)
{
    struct thread_info *info = (struct thread_info *) thread;

    void *retval;
    int status = pthread_join(info->thread, &retval);

    if (status != 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Failed to join thread \"%s\".\n", info->name);
    }

    return (int) ((intptr_t) retval);
}

static libtq_mutex create_mutex(void)
{
    pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));

    if (!mutex) {
        libtq_out_of_memory();
    }

    int status = pthread_mutex_init(mutex, NULL);

    if (status != 0) {
        libtq_error("Failed to create mutex, error code: %d", status);
    }

    return (libtq_mutex) mutex;
}

static void destroy_mutex(libtq_mutex mutex)
{
    int status = pthread_mutex_destroy((pthread_mutex_t *) mutex);

    if (status != 0) {
        libtq_log(LIBTQ_LOG_ERROR, "Failed to destroy mutex, error code: %d", status);
    }

    free(mutex);
}

static void lock_mutex(libtq_mutex mutex)
{
    pthread_mutex_lock((pthread_mutex_t *) mutex);
}

static void unlock_mutex(libtq_mutex mutex)
{
    pthread_mutex_unlock((pthread_mutex_t *) mutex);
}

//------------------------------------------------------------------------------

void libtq_construct_posix_threads(struct libtq_threads_impl *threads)
{
    *threads = (struct libtq_threads_impl) {
        .initialize             = initialize,
        .terminate              = terminate,
        .sleep                  = sleep,
        .create_thread          = create_thread,
        .detach_thread          = detach_thread,
        .wait_thread            = wait_thread,
        .create_mutex           = create_mutex,
        .destroy_mutex          = destroy_mutex,
        .lock_mutex             = lock_mutex,
        .unlock_mutex           = unlock_mutex,
    };
}

//------------------------------------------------------------------------------

#endif // defined(TQ_LINUX) || defined(TQ_ANDROID) || defined(TQ_EMSCRIPTEN)

//------------------------------------------------------------------------------
