#pragma once

#include "fq_poly_types.h"
#include "fq_vec_types.h"
#include <stdbool.h>
#include <stdio.h>

size_t fq_poly_maxdeg(fq_poly_t x);

size_t fq_poly_deg(fq_poly_t x);

size_t fq_poly_mindeg(fq_poly_t x);

fq_poly_t fq_poly_new_empty(size_t deg);

fq_poly_t fq_poly_new(size_t deg);

fq_poly_t fq_poly_new_copy(fq_poly_t x);

fq_poly_t fq_poly_new_view(fq_poly_t x);

void fq_poly_del(fq_poly_t x);

void fq_poly_resize(fq_poly_t *x, size_t deg);

void fq_poly_copy(fq_poly_t *x, fq_poly_t y);

void fq_poly_move(fq_poly_t *x, fq_poly_t y);

fq_poly_t fq_poly_zero();

fq_poly_t fq_poly_one();

fq_poly_t fq_poly_self_add(fq_poly_t x, fq_poly_t y, uint64_t p);

fq_poly_t fq_poly_add(fq_poly_t x, fq_poly_t y, uint64_t p);

fq_poly_t fq_poly_self_neg(fq_poly_t x, uint64_t p);

fq_poly_t fq_poly_neg(fq_poly_t x, uint64_t p);

fq_poly_t fq_poly_self_sub(fq_poly_t x, fq_poly_t y, uint64_t p);

fq_poly_t fq_poly_sub(fq_poly_t x, fq_poly_t y, uint64_t p);

fq_poly_t fq_poly_self_mul(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p);

fq_poly_t fq_poly_mul(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p);

fq_poly_t fq_poly_self_rem(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p);

fq_poly_t fq_poly_rem(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p);

fq_t fq_poly_eval(fq_poly_t x, fq_t a, zp_poly_t r, uint64_t p);

bool fq_poly_is_irred(fq_poly_t x, zp_poly_t r, uint64_t p);

fq_poly_t fq_poly_find_irred(zp_poly_t r, uint64_t p, size_t k);

fq_poly_t fq_poly_from_fq_vec_view(fq_vec_t x);

fq_poly_t fq_poly_from_fq_vec_copy(fq_vec_t x);

fq_poly_t fq_poly_from_str(const char *str);

void fq_poly_print(FILE *stream, fq_poly_t x);
