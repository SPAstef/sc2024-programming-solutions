#pragma once

#include "fq/fq_types.h"
#include "zp_poly_types.h"
#include "zp_vec_types.h"
#include <stdio.h>

zp_vec_t zp_vec_new_empty(size_t n);

zp_vec_t zp_vec_new(size_t n);

zp_vec_t zp_vec_new_copy(zp_vec_t x);

zp_vec_t zp_vec_new_view(zp_vec_t x);

void zp_vec_del(zp_vec_t v);

void zp_vec_resize(zp_vec_t *x, size_t n);

void zp_vec_copy(zp_vec_t *x, zp_vec_t y);

void zp_vec_move(zp_vec_t *x, zp_vec_t y);

zp_vec_t zp_vec_self_add(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_add(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_self_neg(zp_vec_t x, uint64_t p);

zp_vec_t zp_vec_neg(zp_vec_t x, uint64_t p);

zp_vec_t zp_vec_self_sub(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_sub(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_self_smul(zp_vec_t x, zp_t a, uint64_t p);

zp_vec_t zp_vec_smul(zp_vec_t x, zp_t a, uint64_t p);

zp_vec_t zp_vec_self_hmul(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_hmul(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_self_inv(zp_vec_t x, uint64_t p);

zp_vec_t zp_vec_inv(zp_vec_t x, uint64_t p);

zp_vec_t zp_vec_self_sdiv(zp_vec_t x, zp_t a, uint64_t p);

zp_vec_t zp_vec_sdiv(zp_vec_t x, zp_t a, uint64_t p);

zp_vec_t zp_vec_self_hdiv(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_hdiv(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_t zp_vec_sum(zp_vec_t x, uint64_t p);

zp_t zp_vec_prod(zp_vec_t x, uint64_t p);

zp_t zp_vec_dot(zp_vec_t x, zp_vec_t y, uint64_t p);

zp_vec_t zp_vec_self_circmul(zp_vec_t x, zp_vec_t m, uint64_t p);

zp_vec_t zp_vec_circmul(zp_vec_t x, zp_vec_t m, uint64_t p);

zp_vec_t zp_vec_self_reverse(zp_vec_t x);

zp_vec_t zp_vec_reverse(zp_vec_t x);

zp_vec_t zp_vec_from_zp_poly_view(zp_poly_t x);

zp_vec_t zp_vec_from_zp_poly_copy(zp_poly_t x);

zp_vec_t zp_vec_from_fq_view(fq_t x);

zp_vec_t zp_vec_from_fq_copy(fq_t x);

zp_vec_t zp_vec_from_str(const char *str);

void zp_vec_print(FILE *f, zp_vec_t x);
