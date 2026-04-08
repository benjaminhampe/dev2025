#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <stdint.h>

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};

static int clock_gettime(int, struct timespec* ts)
{
    static LARGE_INTEGER freq;
    static BOOL freq_init = FALSE;

    if (!freq_init) {
        QueryPerformanceFrequency(&freq);
        freq_init = TRUE;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    long double seconds = (long double)counter.QuadPart / (long double)freq.QuadPart;

    ts->tv_sec  = (time_t)seconds;
    ts->tv_nsec = (long)((seconds - ts->tv_sec) * 1e9);

    return 0;
}
