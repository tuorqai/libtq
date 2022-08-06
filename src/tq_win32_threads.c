
//------------------------------------------------------------------------------

#if defined(_WIN32)

//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "tq_core.h"
#include "tq_log.h"
#include "tq_mem.h"

//------------------------------------------------------------------------------

#define THREAD_FLAG_WAIT            0x01
#define THREAD_FLAG_DETACHED        0x02

struct ThreadInfo
{
    DWORD               id;
    HANDLE              thread;
    CRITICAL_SECTION    cs;
    UINT                Flags;

    char const          *name;
    int                 (*func)(void *);
    void                *data;
};

//------------------------------------------------------------------------------

/**
 * End point for a thread.
 */
static void thread_end(struct ThreadInfo *info)
{
    // If the thread is detached, then only its info struct should be freed.
    if (!(info->Flags & THREAD_FLAG_DETACHED)) {
        EnterCriticalSection(&info->cs);

        // If the thread is being waited in wait_thread(), do not release
        // its resources.
        if (info->Flags & THREAD_FLAG_WAIT) {
            LeaveCriticalSection(&info->cs);
            return;
        }

        LeaveCriticalSection(&info->cs);

        CloseHandle(info->thread);
        DeleteCriticalSection(&info->cs);
    }

    HeapFree(GetProcessHeap(), 0, info);
}

/**
 * Entry point for a thread.
 */
static DWORD WINAPI thread_main(LPVOID param)
{
    struct ThreadInfo *info = (struct ThreadInfo *) param;
    int retval = info->func(info->data);

    thread_end(info);
    return retval;
}

//------------------------------------------------------------------------------

/**
 * Initialize Win32 multi-threading module.
 */
static void initialize(void)
{
}

/**
 * Terminate Win32 multi-threading module.
 */
static void terminate(void)
{
}

/**
 * Pause current thread for a given period of time.
 */
static void sleep(double seconds)
{
    DWORD milliseconds = (DWORD) (seconds * 1000);
    Sleep(milliseconds);
}

/**
 * Create new thread and return it's opaque handle.
 */
static tq_thread_t create_thread(char const *name, int (*func)(void *), void *data)
{
    struct ThreadInfo *info = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct ThreadInfo));

    info->name = name;
    info->func = func;
    info->data = data;

    info->Flags = 0;
    InitializeCriticalSection(&info->cs);

    info->thread = CreateThread(NULL, 0, thread_main, info, 0, &info->id);

    if (info->thread == NULL) {
        tq_log_error("Failed to create thread \"%s\".\n", name);
        HeapFree(GetProcessHeap(), 0, info);
        return NULL;
    }

    return (tq_thread_t) info;
}

static void detach_thread(tq_thread_t thread)
{
    struct ThreadInfo *info = (struct ThreadInfo *) thread;

    EnterCriticalSection(&info->cs);
    info->Flags |= THREAD_FLAG_DETACHED;
    LeaveCriticalSection(&info->cs);

    CloseHandle(info->thread);
    DeleteCriticalSection(&info->cs);
}

static int wait_thread(tq_thread_t thread)
{
    struct ThreadInfo *info = (struct ThreadInfo *) thread;

    EnterCriticalSection(&info->cs);

    if (info->Flags & THREAD_FLAG_DETACHED) {
        LeaveCriticalSection(&info->cs);
        return -1;
    }

    info->Flags |= THREAD_FLAG_WAIT;
    LeaveCriticalSection(&info->cs);

    DWORD retval;
    WaitForSingleObject(info->thread, INFINITE);
    GetExitCodeThread(info->thread, &retval);

    info->Flags &= ~THREAD_FLAG_WAIT;
    thread_end(info);

    return (int) retval;
}

//------------------------------------------------------------------------------
// Mutexes

static tq_mutex_t create_mutex(void)
{
    LPCRITICAL_SECTION mutex = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CRITICAL_SECTION));
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

#endif // defined(_WIN32)

//------------------------------------------------------------------------------
