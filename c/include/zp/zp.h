#pragma once

#include "zp_types.h"
#include <stdbool.h>

zp_t zp_new(uint64_t x);

zp_t zp_zero();

zp_t zp_one();

uint64_t zp_as_int(zp_t x);

bool zp_is_zero(zp_t x);

zp_t zp_add(zp_t x, zp_t y, uint64_t p);

zp_t zp_neg(zp_t x, uint64_t p);

zp_t zp_sub(zp_t x, zp_t y, uint64_t p);

zp_t zp_mul(zp_t x, zp_t y, uint64_t p);

zp_t zp_pow(zp_t x, uint64_t y, uint64_t p);

zp_t zp_inv(zp_t x, uint64_t p);

zp_t zp_div(zp_t x, zp_t y, uint64_t p);

zp_t zp_rand(uint64_t p);
