#pragma once

#include "fq_poly_types.h"
#include "fq_vec_types.h"

#include <stdio.h>

fq_vec_t fq_vec_new_empty(size_t n);

fq_vec_t fq_vec_new(size_t n);

fq_vec_t fq_vec_new_copy(fq_vec_t x);

fq_vec_t fq_vec_new_view(fq_vec_t x);

void fq_vec_del(fq_vec_t x);

void fq_vec_copy(fq_vec_t *x, fq_vec_t y);

void fq_vec_move(fq_vec_t *x, fq_vec_t y);

fq_vec_t fq_vec_self_add(fq_vec_t x, fq_vec_t y, uint64_t p);

fq_vec_t fq_vec_add(fq_vec_t x, fq_vec_t y, uint64_t p);

fq_vec_t fq_vec_self_neg(fq_vec_t x, uint64_t p);

fq_vec_t fq_vec_neg(fq_vec_t x, uint64_t p);

fq_vec_t fq_vec_self_sub(fq_vec_t x, fq_vec_t y, uint64_t p);

fq_vec_t fq_vec_sub(fq_vec_t x, fq_vec_t y, uint64_t p);

fq_vec_t fq_vec_self_smul(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_smul(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_self_hmul(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_hmul(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_self_inv(fq_vec_t x, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_inv(fq_vec_t x, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_self_sdiv(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_sdiv(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_self_hdiv(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_hdiv(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_t fq_vec_sum(fq_vec_t x, uint64_t p);

fq_t fq_vec_prod(fq_vec_t x, zp_poly_t r, uint64_t p);

fq_t fq_vec_dot(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_vec_self_reverse(fq_vec_t x);

fq_vec_t fq_vec_reverse(fq_vec_t x);

fq_vec_t fq_vec_from_fq_poly_view(fq_poly_t x);

fq_vec_t fq_vec_from_fq_poly_copy(fq_poly_t x);

fq_vec_t fq_vec_from_str(const char *str);

void fq_vec_print(FILE *f, fq_vec_t x);
