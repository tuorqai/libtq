//------------------------------------------------------------------------------
// Copyright (c) 2021-2023 tuorqai
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

#if defined(TQ_WIN32)

//------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "tq_core.h"
#include "tq_log.h"

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
static libtq_thread create_thread(char const *name, int (*func)(void *), void *data)
{
    struct ThreadInfo *info = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct ThreadInfo));

    info->name = name;
    info->func = func;
    info->data = data;

    info->Flags = 0;
    InitializeCriticalSection(&info->cs);

    info->thread = CreateThread(NULL, 0, thread_main, info, 0, &info->id);

    if (info->thread == NULL) {
        libtq_log(LIBTQ_LOG_ERROR, "Failed to create thread \"%s\".\n", name);
        HeapFree(GetProcessHeap(), 0, info);
        return NULL;
    }

    return (libtq_thread) info;
}

static void detach_thread(libtq_thread thread)
{
    struct ThreadInfo *info = (struct ThreadInfo *) thread;

    EnterCriticalSection(&info->cs);
    info->Flags |= THREAD_FLAG_DETACHED;
    LeaveCriticalSection(&info->cs);

    CloseHandle(info->thread);
    DeleteCriticalSection(&info->cs);
}

static int wait_thread(libtq_thread thread)
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

static libtq_mutex create_mutex(void)
{
    LPCRITICAL_SECTION mutex = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CRITICAL_SECTION));
    InitializeCriticalSection(mutex);

    return (libtq_mutex) mutex;
}

static void destroy_mutex(libtq_mutex mutex)
{
    DeleteCriticalSection((LPCRITICAL_SECTION) mutex);
    HeapFree(GetProcessHeap(), 0, mutex);
}

static void lock_mutex(libtq_mutex mutex)
{
    EnterCriticalSection((LPCRITICAL_SECTION) mutex);
}

static void unlock_mutex(libtq_mutex mutex)
{
    LeaveCriticalSection((LPCRITICAL_SECTION) mutex);
}

//------------------------------------------------------------------------------

void libtq_construct_win32_threads(struct libtq_threads_impl *threads)
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

#endif // defined(TQ_WIN32)

//------------------------------------------------------------------------------
