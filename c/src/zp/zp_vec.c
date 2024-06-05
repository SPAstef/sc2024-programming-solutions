#include "zp/zp_vec.h"
#include "fq/fq.h"
#include "zp/zp.h"
#include "zp/zp_poly.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

zp_vec_t zp_vec_new_empty(size_t n)
{
    return (zp_vec_t){.c = malloc(n * sizeof(zp_t)), .n = n};
}

zp_vec_t zp_vec_new(size_t n)
{
    return (zp_vec_t){.c = calloc(n, sizeof(zp_t)), .n = n};
}

zp_vec_t zp_vec_new_copy(zp_vec_t x)
{
    zp_vec_t y = zp_vec_new_empty(x.n);

    memcpy(y.c, x.c, x.n * sizeof *y.c);

    return y;
}

zp_vec_t zp_vec_new_view(zp_vec_t x)
{
    return x;
}

void zp_vec_del(zp_vec_t x)
{
    free(x.c);
}

void zp_vec_resize(zp_vec_t *x, size_t n)
{
    x->c = realloc(x->c, n * sizeof(zp_t));

    for (size_t i = x->n; i < n; ++i)
        x->c[i] = zp_zero();

    x->n = n;
}

void zp_vec_copy(zp_vec_t *x, zp_vec_t y)
{
    zp_vec_del(*x);

    *x = zp_vec_new_copy(y);
}

void zp_vec_move(zp_vec_t *x, zp_vec_t y)
{
    zp_vec_del(*x);

    *x = y;
}

zp_vec_t zp_vec_self_add(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_add(x.c[i], y.c[i], p);

    return x;
}

zp_vec_t zp_vec_add(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    return zp_vec_self_add(zp_vec_new_copy(x), y, p);
}

zp_vec_t zp_vec_self_neg(zp_vec_t x, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_neg(x.c[i], p);

    return x;
}

zp_vec_t zp_vec_neg(zp_vec_t x, uint64_t p)
{
    return zp_vec_self_neg(zp_vec_new_copy(x), p);
}

zp_vec_t zp_vec_self_sub(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_sub(x.c[i], y.c[i], p);

    return x;
}

zp_vec_t zp_vec_sub(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    return zp_vec_self_sub(zp_vec_new_copy(x), y, p);
}

zp_vec_t zp_vec_self_smul(zp_vec_t x, zp_t a, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_mul(x.c[i], a, p);

    return x;
}

zp_vec_t zp_vec_smul(zp_vec_t x, zp_t a, uint64_t p)
{
    return zp_vec_self_smul(zp_vec_new_copy(x), a, p);
}

zp_vec_t zp_vec_self_hmul(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_mul(x.c[i], y.c[i], p);

    return x;
}

zp_vec_t zp_vec_hmul(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    return zp_vec_self_hmul(zp_vec_new_copy(x), y, p);
}

zp_vec_t zp_vec_self_inv(zp_vec_t x, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_inv(x.c[i], p);

    return x;
}

zp_vec_t zp_vec_inv(zp_vec_t x, uint64_t p)
{
    return zp_vec_self_inv(zp_vec_new_copy(x), p);
}

zp_vec_t zp_vec_self_sdiv(zp_vec_t x, zp_t a, uint64_t p)
{
    return zp_vec_self_smul(x, zp_inv(a, p), p);
}

zp_vec_t zp_vec_sdiv(zp_vec_t x, zp_t a, uint64_t p)
{
    return zp_vec_self_sdiv(zp_vec_new_copy(x), a, p);
}

zp_vec_t zp_vec_self_hdiv(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_mul(x.c[i], zp_inv(y.c[i], p), p);

    return x;
}

zp_vec_t zp_vec_hdiv(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    return zp_vec_self_hdiv(zp_vec_new_copy(x), y, p);
}

zp_t zp_vec_sum(zp_vec_t x, uint64_t p)
{
    zp_t z = zp_zero();

    for (size_t i = 0; i < x.n; ++i)
        z = zp_add(z, x.c[i], p);

    return z;
}

zp_t zp_vec_prod(zp_vec_t x, uint64_t p)
{
    zp_t z = zp_one();

    for (size_t i = 0; i < x.n; ++i)
        z = zp_mul(z, x.c[i], p);

    return z;
}

zp_t zp_vec_dot(zp_vec_t x, zp_vec_t y, uint64_t p)
{
    zp_vec_t z = zp_vec_hmul(x, y, p);
    zp_t w = zp_vec_sum(z, p);

    zp_vec_del(z);

    return w;
}

zp_vec_t zp_vec_self_circmul(zp_vec_t x, zp_vec_t m, uint64_t p)
{
    zp_vec_t y = zp_vec_new(x.n);

    for (size_t i = 0; i < y.n; ++i)
        for (size_t j = 0; j < y.n; ++j)
        {
            size_t k = zp_as_int(zp_sub(zp_new(j), zp_new(i), y.n));

            y.c[i] = zp_add(y.c[i], zp_mul(m.c[k], x.c[j], p), p);
        }

    zp_vec_move(&x, y);

    return x;
}

zp_vec_t zp_vec_circmul(zp_vec_t x, zp_vec_t m, uint64_t p)
{
    return zp_vec_self_circmul(zp_vec_new_copy(x), m, p);
}

zp_vec_t zp_vec_self_reverse(zp_vec_t x)
{
    for (size_t i = 0; i < x.n / 2; ++i)
    {
        zp_t t = x.c[i];
        x.c[i] = x.c[x.n - 1 - i];
        x.c[x.n - 1 - i] = t;
    }

    return x;
}

zp_vec_t zp_vec_reverse(zp_vec_t x)
{
    return zp_vec_self_reverse(zp_vec_new_copy(x));
}

zp_vec_t zp_vec_from_zp_poly_view(zp_poly_t x)
{
    return (zp_vec_t){.c = x.c, .n = x.n};
}

zp_vec_t zp_vec_from_zp_poly_copy(zp_poly_t x)
{
    return zp_vec_from_zp_poly_view(zp_poly_new_copy(x));
}

zp_vec_t zp_vec_from_fq_view(fq_t x)
{
    return zp_vec_from_zp_poly_view(fq_to_zp_poly_view(x));
}

zp_vec_t zp_vec_from_fq_copy(fq_t x)
{
    return zp_vec_from_zp_poly_view(fq_to_zp_poly_copy(x));
}

zp_vec_t zp_vec_from_str(const char *str)
{
    size_t n = 1;
    const char *s = str;

    while (*s && *s != ']' && *s != ';')
        if (*s++ == ',')
            ++n;

    zp_vec_t x = zp_vec_new_empty(n);

    for (size_t i = 0; i < n; ++i)
    {
        while (*str && *str != ']' && *str != ';' && !isdigit(*str))
            ++str;
        x.c[i].v = strtoul(str, NULL, 0);
        while (isdigit(*str))
            ++str;
    }

    return x;
}

void zp_vec_print(FILE *f, zp_vec_t x)
{
    fprintf(f, "[");

    if (x.n > 0)
        fprintf(f, "%" PRIu64, zp_as_int(x.c[0]));

    for (size_t i = 1; i < x.n; ++i)
        fprintf(f, ", %" PRIu64, zp_as_int(x.c[i]));

    fprintf(f, "]");
}
