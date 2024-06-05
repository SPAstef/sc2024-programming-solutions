#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
static inline uint8_t ascii_to_hex_digit(char c)
{
    c -= '0';
    if (c > 9)
        c -= 'A' - '0' - 10;
    if (c > 15)
        c -= 'a' - 'A';

    return c;
}

static inline void hexload(uint8_t *buf, size_t sz, const char *str)
{
    size_t i;

    memset(buf, 0, sz);

    for (i = 0; str[2 * i] && str[2 * i + 1] && i < sz; ++i)
        buf[i] = ascii_to_hex_digit(str[2 * i]) << 4 | ascii_to_hex_digit(str[2 * i + 1]);

    if (str[2 * i] && i < sz)
        buf[i] = ascii_to_hex_digit(str[2 * i]) << 4;
}

static inline void hexprint(FILE *stream, const uint8_t *buf, size_t sz)
{
    for (size_t i = 0; i < sz; ++i)
        fprintf(stream, "%02x", buf[i]);
}

#ifdef __cplusplus
}
#endif
