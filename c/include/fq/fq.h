#pragma once

#include "fq_types.h"
#include "zp/zp_vec_types.h"

#include <stdbool.h>
#include <stdio.h>

size_t fq_digits(fq_t x);

fq_t fq_new_empty(size_t dig);

fq_t fq_new(size_t dig);

fq_t fq_new_copy(fq_t x);

fq_t fq_new_view(fq_t x);

void fq_del(fq_t x);

void fq_copy(fq_t *x, fq_t y);

void fq_move(fq_t *x, fq_t y);

fq_t fq_from_zp_poly_view(zp_poly_t x);

fq_t fq_from_zp_poly_copy(zp_poly_t x);

fq_t fq_from_zp_vec_view(zp_vec_t x);

fq_t fq_from_zp_vec_copy(zp_vec_t x);

zp_poly_t fq_to_zp_poly_view(fq_t x);

zp_poly_t fq_to_zp_poly_copy(fq_t x);

fq_t fq_zero();

fq_t fq_one();

bool fq_is_zero(fq_t x);

bool fq_is_one(fq_t x);

fq_t fq_self_add(fq_t x, fq_t y, uint64_t p);

fq_t fq_add(fq_t x, fq_t y, uint64_t p);

fq_t fq_self_neg(fq_t x, uint64_t p);

fq_t fq_neg(fq_t x, uint64_t p);

fq_t fq_self_sub(fq_t x, fq_t y, uint64_t p);

fq_t fq_sub(fq_t x, fq_t y, uint64_t p);

fq_t fq_self_mul(fq_t x, fq_t y, zp_poly_t r, uint64_t p);

fq_t fq_mul(fq_t x, fq_t y, zp_poly_t r, uint64_t p);

fq_t fq_self_pow(fq_t x, uint64_t n, zp_poly_t r, uint64_t p);

fq_t fq_pow(fq_t x, uint64_t n, zp_poly_t r, uint64_t p);

fq_t fq_self_inv(fq_t x, zp_poly_t r, uint64_t p);

fq_t fq_inv(fq_t x, zp_poly_t r, uint64_t p);

fq_t fq_self_div(fq_t x, fq_t y, zp_poly_t r, uint64_t p);

fq_t fq_div(fq_t x, fq_t y, zp_poly_t r, uint64_t p);

fq_t fq_rand(zp_poly_t r, uint64_t p);

uint64_t fq_to_int(fq_t x, uint64_t p);

fq_t fq_from_int(uint64_t x, uint64_t p);

fq_t fq_from_str(const char *str);

void fq_print(FILE *stream, fq_t x);
