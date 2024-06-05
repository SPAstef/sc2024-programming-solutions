#include "fq/fq_vec.h"
#include "fq/fq.h"
#include "fq/fq_poly.h"

#include <ctype.h>
#include <stdlib.h>

fq_vec_t fq_vec_new_empty(size_t n)
{
    return (fq_vec_t){.c = malloc(n * sizeof(fq_t)), .n = n};
}

fq_vec_t fq_vec_new(size_t n)
{
    fq_vec_t x = fq_vec_new_empty(n);

    for (size_t i = 0; i < n; ++i)
        x.c[i] = fq_zero();

    return x;
}

fq_vec_t fq_vec_new_copy(fq_vec_t x)
{
    fq_vec_t y = fq_vec_new_empty(x.n);

    for (size_t i = 0; i < x.n; ++i)
        y.c[i] = fq_new_copy(x.c[i]);

    return y;
}

fq_vec_t fq_vec_new_view(fq_vec_t x)
{
    return x;
}

void fq_vec_del(fq_vec_t x)
{
    for (size_t i = 0; i < x.n; ++i)
        fq_del(x.c[i]);

    free(x.c);
}

void fq_vec_copy(fq_vec_t *x, fq_vec_t y)
{
    fq_vec_del(*x);

    *x = fq_vec_new_copy(y);
}

void fq_vec_move(fq_vec_t *x, fq_vec_t y)
{
    fq_vec_del(*x);

    *x = y;
}

fq_vec_t fq_vec_self_add(fq_vec_t x, fq_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_add(x.c[i], y.c[i], p);

    return x;
}

fq_vec_t fq_vec_add(fq_vec_t x, fq_vec_t y, uint64_t p)
{
    return fq_vec_self_add(fq_vec_new_copy(x), y, p);
}

fq_vec_t fq_vec_self_neg(fq_vec_t x, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_neg(x.c[i], p);

    return x;
}

fq_vec_t fq_vec_neg(fq_vec_t x, uint64_t p)
{
    return fq_vec_self_neg(fq_vec_new_copy(x), p);
}

fq_vec_t fq_vec_self_sub(fq_vec_t x, fq_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_sub(x.c[i], y.c[i], p);

    return x;
}

fq_vec_t fq_vec_sub(fq_vec_t x, fq_vec_t y, uint64_t p)
{
    return fq_vec_self_sub(fq_vec_new_copy(x), y, p);
}

fq_vec_t fq_vec_self_smul(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_mul(x.c[i], a, r, p);

    return x;
}

fq_vec_t fq_vec_smul(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    return fq_vec_self_smul(fq_vec_new_copy(x), a, r, p);
}

fq_vec_t fq_vec_self_hmul(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_mul(x.c[i], y.c[i], r, p);

    return x;
}

fq_vec_t fq_vec_hmul(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    return fq_vec_self_hmul(fq_vec_new_copy(x), y, r, p);
}

fq_vec_t fq_vec_self_inv(fq_vec_t x, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_inv(x.c[i], r, p);

    return x;
}

fq_vec_t fq_vec_inv(fq_vec_t x, zp_poly_t r, uint64_t p)
{
    return fq_vec_self_inv(fq_vec_new_copy(x), r, p);
}

fq_vec_t fq_vec_self_sdiv(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    fq_t a_inv = fq_inv(a, r, p);

    x = fq_vec_self_smul(x, a_inv, r, p);
    fq_del(a_inv);

    return x;
}

fq_vec_t fq_vec_sdiv(fq_vec_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    return fq_vec_self_sdiv(fq_vec_new_copy(x), a, r, p);
}

fq_vec_t fq_vec_self_hdiv(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_div(x.c[i], y.c[i], r, p);

    return x;
}

fq_vec_t fq_vec_hdiv(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    return fq_vec_self_hdiv(fq_vec_new_copy(x), y, r, p);
}

fq_t fq_vec_sum(fq_vec_t x, uint64_t p)
{
    fq_t z = fq_zero();

    for (size_t i = 0; i < x.n; ++i)
        z = fq_self_add(z, x.c[i], p);

    return z;
}

fq_t fq_vec_prod(fq_vec_t x, zp_poly_t r, uint64_t p)
{
    fq_t z = fq_one();

    for (size_t i = 0; i < x.n; ++i)
        z = fq_self_mul(z, x.c[i], r, p);

    return z;
}

fq_t fq_vec_dot(fq_vec_t x, fq_vec_t y, zp_poly_t r, uint64_t p)
{
    fq_vec_t z = fq_vec_hmul(x, y, r, p);

    fq_t w = fq_vec_sum(z, p);

    fq_vec_del(z);

    return w;
}

fq_vec_t fq_vec_self_reverse(fq_vec_t x)
{
    for (size_t i = 0; i < x.n / 2; ++i)
    {
        fq_t t = x.c[i];
        x.c[i] = x.c[x.n - i - 1];
        x.c[x.n - i - 1] = t;
    }

    return x;
}

fq_vec_t fq_vec_reverse(fq_vec_t x)
{
    return fq_vec_self_reverse(fq_vec_new_copy(x));
}

fq_vec_t fq_vec_from_fq_poly_view(fq_poly_t x)
{
    return (fq_vec_t){.c = x.c, .n = x.n};
}

fq_vec_t fq_vec_from_fq_poly_copy(fq_poly_t x)
{
    return fq_vec_from_fq_poly_view(fq_poly_new_copy(x));
}

fq_vec_t fq_vec_from_str(const char *str)
{
    size_t n = 1;
    const char *s = str;

    while (*s)
        if (*s++ == ';')
            ++n;

    fq_vec_t x = fq_vec_new_empty(n);

    for (size_t i = 0; i < n; ++i)
    {
        while (*str && !isdigit(*str))
            ++str;
        x.c[i] = fq_from_str(str);
        while (*str && *str != ';')
            ++str;
    }

    return x;
}

void fq_vec_print(FILE *f, fq_vec_t x)
{
    fprintf(f, "[");
    for (size_t i = 0; i < x.n; ++i)
    {
        fq_print(f, x.c[i]);
        if (i < x.n - 1)
            fprintf(f, ", ");
    }
    fprintf(f, "]");
}
