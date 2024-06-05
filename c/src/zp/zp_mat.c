#include "zp/zp_mat.h"
#include "zp/zp.h"
#include "zp/zp_vec.h"

#include <stdlib.h>
#include <string.h>

zp_mat_t zp_mat_new_empty(size_t rows, size_t cols)
{
    return (zp_mat_t){.c = malloc(rows * cols * sizeof(zp_t)), .rows = rows, .cols = cols};
}

zp_mat_t zp_mat_new(size_t rows, size_t cols)
{
    return (zp_mat_t){.c = calloc(rows * cols, sizeof(zp_t)), .rows = rows, .cols = cols};
}

zp_mat_t zp_mat_new_copy(zp_mat_t x)
{
    zp_mat_t y = zp_mat_new_empty(x.rows, x.cols);

    memcpy(y.c, x.c, x.rows * x.cols * sizeof *y.c);

    return y;
}

zp_mat_t zp_mat_new_view(zp_mat_t x)
{
    return x;
}

void zp_mat_del(zp_mat_t x)
{
    free(x.c);
}

void zp_mat_copy(zp_mat_t *x, zp_mat_t y)
{
    zp_mat_del(*x);

    *x = zp_mat_new_copy(y);
}

void zp_mat_move(zp_mat_t *x, zp_mat_t y)
{
    zp_mat_del(*x);

    *x = y;
}

zp_vec_t zp_mat_row_view(zp_mat_t x, size_t i)
{
    zp_vec_t v = {.c = x.c + i * x.cols, .n = x.cols};

    return v;
}

zp_mat_t zp_mat_self_trans(zp_mat_t x)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < i; ++j)
        {
            zp_t t = x.c[i * x.cols + j];
            x.c[i * x.cols + j] = x.c[j * x.cols + i];
            x.c[j * x.cols + i] = t;
        }

    size_t t = x.rows;
    x.rows = x.cols;
    x.cols = t;

    return x;
}

zp_mat_t zp_mat_trans(zp_mat_t x)
{
    return zp_mat_self_trans(zp_mat_new_copy(x));
}

zp_mat_t zp_mat_self_add(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_add(x.c[i * x.cols + j], y.c[i * x.cols + j], p);

    return x;
}

zp_mat_t zp_mat_add(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    return zp_mat_self_add(zp_mat_new_copy(x), y, p);
}

zp_mat_t zp_mat_self_neg(zp_mat_t x, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_neg(x.c[i * x.cols + j], p);

    return x;
}

zp_mat_t zp_mat_neg(zp_mat_t x, uint64_t p)
{
    return zp_mat_self_neg(zp_mat_new_copy(x), p);
}

zp_mat_t zp_mat_self_sub(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_sub(x.c[i * x.cols + j], y.c[i * x.cols + j], p);

    return x;
}

zp_mat_t zp_mat_sub(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    return zp_mat_self_sub(zp_mat_new_copy(x), y, p);
}

zp_mat_t zp_mat_self_smul(zp_mat_t x, zp_t a, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_mul(x.c[i * x.cols + j], a, p);

    return x;
}

zp_mat_t zp_mat_smul(zp_mat_t x, zp_t a, uint64_t p)
{
    return zp_mat_self_smul(zp_mat_new_copy(x), a, p);
}

zp_mat_t zp_mat_self_hmul(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_mul(x.c[i * x.cols + j], y.c[i * x.cols + j], p);

    return x;
}

zp_mat_t zp_mat_hmul(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    return zp_mat_self_hmul(zp_mat_new_copy(x), y, p);
}

zp_mat_t zp_mat_mul(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    zp_mat_t t = zp_mat_trans(y);
    zp_mat_t z = zp_mat_new_empty(x.rows, t.rows);

    for (size_t i = 0; i < z.rows; ++i)
        for (size_t j = 0; j < z.cols; ++j)
            z.c[i * z.cols + j] = zp_vec_dot(zp_mat_row_view(x, i), zp_mat_row_view(t, j), p);

    return z;
}

zp_vec_t zp_mat_vmul(zp_mat_t x, zp_vec_t y, uint64_t p)
{
    zp_vec_t z = zp_vec_new_empty(x.rows);

    for (size_t i = 0; i < z.n; ++i)
        z.c[i] = zp_vec_dot(zp_mat_row_view(x, i), y, p);

    return z;
}

zp_mat_t zp_mat_self_hinv(zp_mat_t x, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_inv(x.c[i * x.cols + j], p);

    return x;
}

zp_mat_t zp_mat_hinv(zp_mat_t x, uint64_t p)
{
    return zp_mat_self_hinv(zp_mat_new_copy(x), p);
}

zp_mat_t zp_mat_self_sdiv(zp_mat_t x, zp_t a, uint64_t p)
{
    return zp_mat_self_smul(x, zp_inv(a, p), p);
}

zp_mat_t zp_mat_sdiv(zp_mat_t x, zp_t a, uint64_t p)
{
    return zp_mat_smul(x, zp_inv(a, p), p);
}

zp_mat_t zp_mat_self_hdiv(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    for (size_t i = 0; i < x.rows; ++i)
        for (size_t j = 0; j < x.cols; ++j)
            x.c[i * x.cols + j] = zp_mul(x.c[i * x.cols + j], zp_inv(y.c[i * x.cols + j], p), p);

    return x;
}

zp_mat_t zp_mat_hdiv(zp_mat_t x, zp_mat_t y, uint64_t p)
{
    return zp_mat_self_hdiv(zp_mat_new_copy(x), y, p);
}

zp_vec_t zp_mat_vdiv(zp_mat_t x, zp_vec_t y, uint64_t p)
{
    zp_vec_t z = zp_vec_inv(y, p);

    zp_vec_move(&z, zp_mat_vmul(x, z, p));

    return z;
}

void zp_mat_print(FILE *stream, zp_mat_t x)
{
    for (size_t i = 0; i < x.rows; ++i)
    {
        zp_vec_print(stream, zp_mat_row_view(x, i));
        fputc('\n', stream);
    }
}
