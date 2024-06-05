#pragma once

#include "zp_vec_types.h"
#include "zp_mat_types.h"

#include <stdio.h>

zp_mat_t zp_mat_new(size_t rows, size_t cols);

zp_mat_t zp_mat_new_copy(zp_mat_t m);

zp_mat_t zp_mat_new_view(zp_mat_t m);

void zp_mat_del(zp_mat_t m);

void zp_mat_copy(zp_mat_t *x, zp_mat_t y);

void zp_mat_move(zp_mat_t *x, zp_mat_t y);

zp_vec_t zp_mat_row_view(zp_mat_t x, size_t i);

zp_mat_t zp_mat_self_trans(zp_mat_t x);

zp_mat_t zp_mat_trans(zp_mat_t x);

zp_mat_t zp_mat_self_add(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_add(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_self_neg(zp_mat_t x, uint64_t p);

zp_mat_t zp_mat_neg(zp_mat_t x, uint64_t p);

zp_mat_t zp_mat_self_sub(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_sub(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_self_smul(zp_mat_t x, zp_t a, uint64_t p);

zp_mat_t zp_mat_smul(zp_mat_t x, zp_t a, uint64_t p);

zp_mat_t zp_mat_self_hmul(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_hmul(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_mat_t zp_mat_mul(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_vec_t zp_mat_vmul(zp_mat_t x, zp_vec_t y, uint64_t p);

zp_mat_t zp_mat_self_hinv(zp_mat_t x, uint64_t p);

zp_mat_t zp_mat_hinv(zp_mat_t x, uint64_t p);

zp_mat_t zp_mat_self_sdiv(zp_mat_t x, zp_t a, uint64_t p);

zp_mat_t zp_mat_sdiv(zp_mat_t x, zp_t a, uint64_t p);

zp_mat_t zp_mat_hdiv(zp_mat_t x, zp_mat_t y, uint64_t p);

zp_vec_t zp_mat_vdiv(zp_mat_t x, zp_vec_t y, uint64_t p);

void zp_mat_print(FILE *stream, zp_mat_t x);
