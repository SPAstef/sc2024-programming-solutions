#pragma once

#include "fq_mat_types.h"
#include "fq_vec_types.h"

#include <stdio.h>

fq_mat_t fq_mat_new_empty(size_t rows, size_t cols);

fq_mat_t fq_mat_new(size_t rows, size_t cols);

fq_mat_t fq_mat_new_copy(fq_mat_t x);

fq_mat_t fq_mat_new_view(fq_mat_t x);

void fq_mat_del(fq_mat_t x);

void fq_mat_copy(fq_mat_t *x, fq_mat_t y);

void fq_mat_move(fq_mat_t *x, fq_mat_t y);

fq_vec_t fq_mat_row_view(fq_mat_t x, size_t i);

fq_mat_t fq_mat_self_trans(fq_mat_t x);

fq_mat_t fq_mat_trans(fq_mat_t x);

fq_mat_t fq_mat_self_add(fq_mat_t x, fq_mat_t y, uint64_t p);

fq_mat_t fq_mat_add(fq_mat_t x, fq_mat_t y, uint64_t p);

fq_mat_t fq_mat_self_neg(fq_mat_t x, uint64_t p);

fq_mat_t fq_mat_neg(fq_mat_t x, uint64_t p);

fq_mat_t fq_mat_self_sub(fq_mat_t x, fq_mat_t y, uint64_t p);

fq_mat_t fq_mat_sub(fq_mat_t x, fq_mat_t y, uint64_t p);

fq_mat_t fq_mat_self_smul(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_smul(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_self_hmul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_hmul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_mul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_mat_vmul(fq_mat_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_self_hinv(fq_mat_t x, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_hinv(fq_mat_t x, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_self_sdiv(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_sdiv(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_self_hdiv(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p);

fq_mat_t fq_mat_hdiv(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p);

fq_vec_t fq_mat_vdiv(fq_mat_t x, fq_vec_t y, zp_poly_t r, uint64_t p);

void fq_mat_print(FILE *stream, fq_mat_t x);
