// Power C Compatibility
// How come there is no delay function in power c?

#ifndef POWERC
This file should ONLY be compiled when compiling for Power C.
#endif

#include <time.h>

void    delay(long milliseconds)
{
    extern  clock_t clock(void);

    clock_t start;

    start = clock;
    while ((clock() - start) < milliseconds);
}
