
//------------------------------------------------------------------------------

#if defined(TQ_PLATFORM_WINDOWS)

//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------

struct thread_info
{
    DWORD       id;
    HANDLE      thread;

    char const  *name;
    int         (*func)(void *);
    void        *data;

    CRITICAL_SECTION    mutex;
    bool                bits;
};

//------------------------------------------------------------------------------

static DWORD WINAPI thread_main(LPVOID param)
{
    struct thread_info *info = (struct thread_info *) param;
    int retval = info->func(info->data);

    EnterCriticalSection(&info->mutex);

    // We are waiting for this thread in wait_thread(), don't delete the stuff
    if (info->bits & 0x01) {
        LeaveCriticalSection(&info->mutex);
        return retval;
    }

    // Close the thread handle if it's not detached
    if ((info->bits & 0x02) == 0) {
        CloseHandle(info->thread);
    }

    LeaveCriticalSection(&info->mutex);
    DeleteCriticalSection(&info->mutex);
    tq_mem_free(info);

    return retval;
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
    DWORD dwMilliseconds = (DWORD) (seconds * 1000);
    Sleep(dwMilliseconds);
}

static tq_thread_t create_thread(char const *name, int (*func)(void *), void *data)
{
    struct thread_info *info = tq_mem_alloc(sizeof(thread_info));

    info->name = name;
    info->func = func;
    info->data = data;

    info->thread = CreateThread(
        NULL,
        0,
        thread_main,
        info,
        0,
        &info->id
    );

    if (info->thread == NULL) {
        tq_log_error("Failed to create thread \"%s\".\n", name);
        tq_mem_free(info);
        return NULL;
    }

    InitializeCriticalSection(&info->mutex);
    info->bits = 0;

    return (tq_thread_t) info;
}

static void detach_thread(tq_thread_t thread)
{
    struct thread_info *info = (struct thread_info *) param;

    EnterCriticalSection(&info->mutex);
    info->bits |= 0x02;
    LeaveCriticalSection(&info->mutex);

    CloseHandle(info->thread);
}

static int wait_thread(tq_thread_t thread)
{
    struct thread_info *info = (struct thread_info *) param;

    // The thread is detached
    if (info->bits & 0x02) {
        return -1;
    }

    EnterCriticalSection(&info->mutex);
    info->bits |= 0x01;
    LeaveCriticalSection(&info->mutex);

    WaitForSingleObject(info->thread, INFINITE);

    DWORD retval;
    GetExitCodeThread(info->thread, &retval);

    DeleteCriticalSection(&info->mutex);
    CloseHandle(info->thread);

    tq_mem_free(info);

    return (int) retval;
}

static tq_mutex_t create_mutex(void)
{
    LPCRITICAL_SECTION mutex = tq_mem_alloc(sizeof(CRITICAL_SECTION));

    InitializeCriticalSection(mutex);

    return (tq_mutex_t) mutex;
}

static void destroy_mutex(tq_mutex_t mutex)
{
    DeleteCriticalSection((LPCRITICAL_SECTION) mutex);
    tq_mem_free(mutex);
}

static void lock_mutex(tq_mutex_t mutex)
{
    EnterCriticalSection((LPCRITICAL_SECTION) mutex);
}

static void unlock_mutex(tq_mutex_t mutex)
{
    LeaveCriticalSection((LPCRITICAL_SECTION) mutex);
}

//------------------------------------------------------------------------------

void tq_construct_win32_threads(tq_threads_impl_t *impl)
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

#endif // defined(TQ_PLATFORM_WINDOWS)

//------------------------------------------------------------------------------
