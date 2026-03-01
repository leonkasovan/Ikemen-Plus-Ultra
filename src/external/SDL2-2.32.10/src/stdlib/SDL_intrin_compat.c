/* Compatibility wrappers for MSVC intrinsic names used by SDL_atomic.c
   Provides both single- and double-underscore variants of the pointer
   interlocked intrinsics by forwarding to the Windows Interlocked* API. */
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Double-underscore variants (older toolchains / intrinsics) */
void * __cdecl __InterlockedCompareExchangePointer(void * volatile *Destination, void *Exchange, void *Comparand)
{
    return InterlockedCompareExchangePointer(Destination, Exchange, Comparand);
}

void * __cdecl __InterlockedExchangePointer(void * volatile *Target, void *Value)
{
    return InterlockedExchangePointer(Target, Value);
}

/* Single-underscore variants used in SDL_atomic.c */
void * __cdecl _InterlockedCompareExchangePointer(void * volatile *Destination, void *Exchange, void *Comparand)
{
    return InterlockedCompareExchangePointer(Destination, Exchange, Comparand);
}

void * __cdecl _InterlockedExchangePointer(void * volatile *Target, void *Value)
{
    return InterlockedExchangePointer(Target, Value);
}

#ifdef __cplusplus
}
#endif
