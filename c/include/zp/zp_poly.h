#pragma once

#include "zp_poly_types.h"
#include "zp_vec_types.h"
#include <stdbool.h>
#include <stdio.h>

size_t zp_poly_maxdeg(zp_poly_t x);

size_t zp_poly_deg(zp_poly_t x);

size_t zp_poly_mindeg(zp_poly_t x);

zp_poly_t zp_poly_new_empty(size_t deg);

zp_poly_t zp_poly_new(size_t deg);

zp_poly_t zp_poly_new_copy(zp_poly_t x);

zp_poly_t zp_poly_new_view(zp_poly_t x);

void zp_poly_del(zp_poly_t x);

void zp_poly_resize(zp_poly_t *x, size_t deg);

void zp_poly_copy(zp_poly_t *x, zp_poly_t y);

void zp_poly_move(zp_poly_t *x, zp_poly_t y);

zp_poly_t zp_poly_zero();

zp_poly_t zp_poly_one();

zp_poly_t zp_poly_self_add(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_add(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_self_neg(zp_poly_t x, uint64_t p);

zp_poly_t zp_poly_neg(zp_poly_t x, uint64_t p);

zp_poly_t zp_poly_self_sub(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_sub(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_self_mul(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_mul(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_self_rem(zp_poly_t x, zp_poly_t y, uint64_t p);

zp_poly_t zp_poly_rem(zp_poly_t x, zp_poly_t y, uint64_t p);

bool zp_poly_is_zero(zp_poly_t x);

bool zp_poly_is_one(zp_poly_t x);

zp_t zp_poly_eval(zp_poly_t x, zp_t a, uint64_t p);

bool zp_poly_is_irred(zp_poly_t x, uint64_t p);

zp_poly_t zp_poly_find_irred(uint64_t p, size_t k);

uint64_t zp_poly_to_int(zp_poly_t x, uint64_t p);

zp_poly_t zp_poly_from_int(uint64_t x, uint64_t p);

zp_poly_t zp_poly_from_zp_vec_view(zp_vec_t x);

zp_poly_t zp_poly_from_zp_vec_copy(zp_vec_t x);

zp_poly_t zp_poly_from_str(const char *str);

void zp_poly_print(FILE *stream, zp_poly_t x);
