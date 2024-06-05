#include "fq/fq_mat.h"
#include "fq/fq.h"
#include "fq/fq_vec.h"

#include <stdlib.h>

fq_mat_t fq_mat_new_empty(size_t rows, size_t cols)
{
    return (fq_mat_t){.c = malloc(rows * cols * sizeof(fq_t)), .rows = rows, .cols = cols};
}

fq_mat_t fq_mat_new(size_t rows, size_t cols)
{
    fq_mat_t x = fq_mat_new_empty(rows, cols);

    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            x.c[i * cols + j] = fq_zero();

    return x;
}

fq_mat_t fq_mat_new_copy(fq_mat_t x)
{
    fq_mat_t y = fq_mat_new_empty(x.rows, x.cols);

    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            y.c[i * y.cols + j] = fq_new_copy(x.c[i * x.cols + j]);

    return y;
}

fq_mat_t fq_mat_new_view(fq_mat_t x)
{
    return x;
}

void fq_mat_del(fq_mat_t x)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            fq_del(x.c[i * x.cols + j]);

    free(x.c);
}

void fq_mat_copy(fq_mat_t *x, fq_mat_t y)
{
    fq_mat_del(*x);

    *x = fq_mat_new_copy(y);
}

void fq_mat_move(fq_mat_t *x, fq_mat_t y)
{
    fq_mat_del(*x);

    *x = y;
}

fq_vec_t fq_mat_row_view(fq_mat_t x, size_t i)
{
    fq_vec_t v = {.c = x.c + i * x.cols, .n = x.cols};

    return v;
}

fq_mat_t fq_mat_self_trans(fq_mat_t x)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            fq_t t = x.c[i * x.cols + j];

            x.c[i * x.cols + j] = x.c[j * x.cols + i];
            x.c[j * x.cols + i] = t;
        }

    size_t t = x.rows;
    x.rows = x.cols;
    x.cols = t;

    return x;
}

fq_mat_t fq_mat_trans(fq_mat_t x)
{
    return fq_mat_self_trans(fq_mat_new_copy(x));
}

fq_mat_t fq_mat_self_add(fq_mat_t x, fq_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_add(x.c[i * x.cols + j], y.c[i * x.cols + j], p);

    return x;
}

fq_mat_t fq_mat_add(fq_mat_t x, fq_mat_t y, uint64_t p)
{
    return fq_mat_self_add(fq_mat_new_copy(x), y, p);
}

fq_mat_t fq_mat_self_neg(fq_mat_t x, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_neg(x.c[i * x.cols + j], p);

    return x;
}

fq_mat_t fq_mat_neg(fq_mat_t x, uint64_t p)
{
    return fq_mat_self_neg(fq_mat_new_copy(x), p);
}

fq_mat_t fq_mat_self_sub(fq_mat_t x, fq_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_sub(x.c[i * x.cols + j], y.c[i * x.cols + j], p);

    return x;
}

fq_mat_t fq_mat_sub(fq_mat_t x, fq_mat_t y, uint64_t p)
{
    return fq_mat_self_sub(fq_mat_new_copy(x), y, p);
}

fq_mat_t fq_mat_self_smul(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_mul(x.c[i * x.cols + j], a, r, p);

    return x;
}

fq_mat_t fq_mat_smul(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    return fq_mat_self_smul(fq_mat_new_copy(x), a, r, p);
}

fq_mat_t fq_mat_self_hmul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_mul(x.c[i * x.cols + j], y.c[i * x.cols + j], r, p);

    return x;
}

fq_mat_t fq_mat_hmul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p)
{
    return fq_mat_self_hmul(fq_mat_new_copy(x), y, r, p);
}

fq_mat_t fq_mat_mul(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p)
{
    fq_mat_t t = fq_mat_trans(y);
    fq_mat_t z = fq_mat_new_empty(x.rows, t.rows);

    for (size_t i = 0; i < z.rows; ++i)
        for (size_t j = 0; j < z.cols; ++j)
            z.c[i * z.cols + j] = fq_vec_dot(fq_mat_row_view(x, i), fq_mat_row_view(t, j), r, p);

    return z;
}

fq_vec_t fq_mat_vmul(fq_mat_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    fq_vec_t z = fq_vec_new_empty(x.rows);

    for (size_t i = 0; i < z.n; ++i)
        z.c[i] = fq_vec_dot(fq_mat_row_view(x, i), y, r, p);

    return z;
}

fq_mat_t fq_mat_self_hinv(fq_mat_t x, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_inv(x.c[i * x.cols + j], r, p);

    return x;
}

fq_mat_t fq_mat_hinv(fq_mat_t x, zp_poly_t r, uint64_t p)
{
    return fq_mat_self_hinv(fq_mat_new_copy(x), r, p);
}

fq_mat_t fq_mat_self_sdiv(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    fq_t t = fq_inv(a, r, p);

    x = fq_mat_self_smul(x, t, r, p);
    fq_del(t);

    return x;
}

fq_mat_t fq_mat_sdiv(fq_mat_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    return fq_mat_self_sdiv(fq_mat_new_copy(x), a, r, p);
}

fq_mat_t fq_mat_self_hdiv(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = fq_self_div(x.c[i * x.cols + j], y.c[i * x.cols + j], r, p);

    return x;
}

fq_mat_t fq_mat_hdiv(fq_mat_t x, fq_mat_t y, zp_poly_t r, uint64_t p)
{
    return fq_mat_self_hdiv(fq_mat_new_copy(x), y, r, p);
}

fq_vec_t fq_mat_vdiv(fq_mat_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    fq_vec_t z = fq_vec_inv(y, r, p);

    fq_vec_move(&z, fq_mat_vmul(x, z, r, p));

    return z;
}

void fq_mat_print(FILE *stream, fq_mat_t x)
{
    for (size_t i = 0; i < x.rows; ++i)
    {
        fq_vec_print(stream, fq_mat_row_view(x, i));
        fputc('\n', stream);
    }
}
