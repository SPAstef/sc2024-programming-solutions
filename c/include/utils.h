#pragma once

#include <stdint.h>

#ifndef min
    #define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
    #define max(a, b) ((a) > (b) ? (a) : (b))
#endif

static inline uint64_t pow64(uint64_t x, uint64_t y)
{
    uint64_t z = 1;

    while (y)
    {
        if (y & 1)
            z *= x;
        x *= x;
        y >>= 1;
    }

    return z;
}
