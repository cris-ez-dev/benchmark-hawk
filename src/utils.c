// utils.c
#include "utils.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static LARGE_INTEGER _perf_freq;

void timer_init(void) {
    QueryPerformanceFrequency(&_perf_freq);
}

uint64_t obtener_tiempo_microsegundos(void) {
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    // escala a microsegundos
    return (uint64_t)(t.QuadPart * 1000000ull / _perf_freq.QuadPart);
}

#else  // POSIX (Linux, macOS…)

#include <time.h>

void timer_init(void) {
    // no‐op en POSIX
}

uint64_t obtener_tiempo_microsegundos(void) {
    struct timespec ts;
    // CLOCK_MONOTONIC garantiza un reloj no retrocedente
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ull + (ts.tv_nsec / 1000);
}

#endif