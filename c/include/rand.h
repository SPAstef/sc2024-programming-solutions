#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t prand32(uint32_t lo, uint32_t hi);

uint64_t prand64(uint64_t lo, uint64_t hi);

void randbytes(uint8_t *buf, size_t len);

#ifdef __cplusplus
}
#endif
