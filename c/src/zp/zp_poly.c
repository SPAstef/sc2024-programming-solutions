#include "zp/zp_poly.h"
#include "utils.h"
#include "zp/zp.h"
#include "zp/zp_vec.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

size_t zp_poly_maxdeg(zp_poly_t x)
{
    return x.n - 1;
}

size_t zp_poly_deg(zp_poly_t x)
{
    for (size_t i = x.n - 1; i; --i)
        if (x.c[i].v)
            return i;

    return 0;
}

size_t zp_poly_mindeg(zp_poly_t x)
{
    for (size_t i = 0; i < x.n; ++i)
        if (x.c[i].v)
            return i;

    return x.n;
}

zp_poly_t zp_poly_new_empty(size_t deg)
{
    return (zp_poly_t){.c = malloc((deg + 1) * sizeof(zp_t)), .n = deg + 1};
}

zp_poly_t zp_poly_new(size_t deg)
{
    return (zp_poly_t){.c = calloc(deg + 1, sizeof(zp_t)), .n = deg + 1};
}

zp_poly_t zp_poly_new_copy(zp_poly_t x)
{
    zp_poly_t y = zp_poly_new_empty(zp_poly_maxdeg(x));

    memcpy(y.c, x.c, y.n * sizeof(zp_t));

    return y;
}

zp_poly_t zp_poly_new_view(zp_poly_t x)
{
    return x;
}

void zp_poly_del(zp_poly_t x)
{
    free(x.c);
}

void zp_poly_copy(zp_poly_t *x, zp_poly_t y)
{
    zp_poly_del(*x);

    *x = zp_poly_new_copy(y);
}

void zp_poly_move(zp_poly_t *x, zp_poly_t y)
{
    zp_poly_del(*x);

    *x = y;
}

zp_poly_t zp_poly_zero()
{
    zp_poly_t x = zp_poly_new_empty(0);

    x.c[0] = zp_zero();

    return x;
}

zp_poly_t zp_poly_one()
{
    zp_poly_t x = zp_poly_new(0);

    x.c[0] = zp_one();

    return x;
}

void zp_poly_resize(zp_poly_t *x, size_t deg)
{
    x->c = realloc(x->c, (deg + 1) * sizeof(zp_t));

    for (size_t i = x->n; i <= deg; ++i)
        x->c[i] = zp_zero();

    x->n = deg + 1;
}

zp_poly_t zp_poly_self_add(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    size_t d_x = zp_poly_maxdeg(x);
    size_t d_y = zp_poly_deg(y);

    if (d_x < d_y)
        zp_poly_resize(&x, d_y);

    for (size_t i = 0; i <= d_y; ++i)
        x.c[i] = zp_add(x.c[i], y.c[i], p);

    return x;
}

zp_poly_t zp_poly_add(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    return zp_poly_self_add(zp_poly_new_copy(x), y, p);
}

zp_poly_t zp_poly_self_neg(zp_poly_t x, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = zp_neg(x.c[i], p);

    return x;
}

zp_poly_t zp_poly_neg(zp_poly_t x, uint64_t p)
{
    return zp_poly_self_neg(zp_poly_new_copy(x), p);
}

zp_poly_t zp_poly_self_sub(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    if (x.n < y.n)
        zp_poly_resize(&x, zp_poly_maxdeg(y));

    for (size_t i = 0; i < y.n; ++i)
        x.c[i] = zp_sub(x.c[i], y.c[i], p);

    return x;
}

zp_poly_t zp_poly_sub(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    return zp_poly_self_sub(zp_poly_new_copy(x), y, p);
}

zp_poly_t zp_poly_self_mul(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    size_t d_x = zp_poly_deg(x);
    size_t d_y = zp_poly_deg(y);
    zp_poly_t z = zp_poly_new(d_x + d_y);

    for (size_t i = 0; i <= d_x; ++i)
        for (size_t j = 0; j <= d_y; ++j)
            z.c[i + j] = zp_add(z.c[i + j], zp_mul(x.c[i], y.c[j], p), p);

    zp_poly_move(&x, z);

    return x;
}

zp_poly_t zp_poly_mul(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    return zp_poly_self_mul(zp_poly_new_copy(x), y, p);
}

zp_poly_t zp_poly_self_rem(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    size_t d_x = zp_poly_deg(x);
    size_t d_y = zp_poly_deg(y);

    for (size_t i = d_x; i >= d_y; --i)
    {
        zp_t c = zp_div(x.c[i], y.c[d_y], p);

        for (size_t j = 0; j <= d_y; ++j)
            x.c[i - j] = zp_sub(x.c[i - j], zp_mul(y.c[d_y - j], c, p), p);
    }

    return x;
}

zp_poly_t zp_poly_rem(zp_poly_t x, zp_poly_t y, uint64_t p)
{
    return zp_poly_self_rem(zp_poly_new_copy(x), y, p);
}

bool zp_poly_is_zero(zp_poly_t x)
{
    for (size_t i = 0; i < x.n; ++i)
        if (x.c[i].v)
            return false;

    return true;
}

bool zp_poly_is_one(zp_poly_t x)
{
    if (x.c[0].v != 1)
        return false;

    for (size_t i = 1; i < x.n; ++i)
        if (x.c[i].v)
            return false;

    return true;
}

zp_t zp_poly_eval(zp_poly_t x, zp_t a, uint64_t p)
{
    size_t d = zp_poly_maxdeg(x);
    zp_t r = x.c[d];

    for (size_t i = 1; i <= d; ++i)
        r = zp_add(zp_mul(r, a, p), x.c[d - i], p);

    return r;
}

bool zp_poly_is_irred(zp_poly_t x, uint64_t p)
{
    for (size_t i = 0; i < p; ++i)
        if (zp_poly_eval(x, zp_new(i), p).v == 0)
            return false;

    return true;
}

zp_poly_t zp_poly_find_irred(uint64_t p, size_t k)
{
    // k >= 2
    zp_poly_t r = zp_poly_new(k);

    r.c[0] = zp_one(); // constant term must be non-zero
    r.c[k] = zp_one(); // enforce degree k

    do
        for (size_t i = 1; i < k; ++i)
            r.c[i] = zp_rand(p);
    while (!zp_poly_is_irred(r, p));


    return r;
}

uint64_t zp_poly_to_int(zp_poly_t x, uint64_t p)
{
    uint64_t y = 0;
    size_t d = zp_poly_maxdeg(x);

    for (size_t i = 0; i <= d; ++i)
        y = y * p + x.c[d - i].v;

    return y;
}

zp_poly_t zp_poly_from_int(uint64_t x, uint64_t p)
{
    uint64_t d = 0;

    for (uint64_t t = x / p; t; t /= p)
        ++d;

    zp_poly_t y = zp_poly_new(d);

    for (uint64_t i = 0; x; x /= p, ++i)
        y.c[i] = zp_new(x % p);

    return y;
}

zp_poly_t zp_poly_from_zp_vec_view(zp_vec_t x)
{
    return (zp_poly_t){.c = x.c, .n = x.n};
}

zp_poly_t zp_poly_from_zp_vec_copy(zp_vec_t x)
{
    return zp_poly_from_zp_vec_view(zp_vec_new_copy(x));
}

zp_poly_t zp_poly_from_str(const char *str)
{
    return zp_poly_from_zp_vec_view(zp_vec_self_reverse(zp_vec_from_str(str)));
}

void zp_poly_print(FILE *stream, zp_poly_t x)
{
    bool first = true;

    for (size_t i = zp_poly_maxdeg(x); i > 0; --i)
    {
        if (x.c[i].v == 0)
            continue;

        if (!first)
            fprintf(stream, " + ");
        first = false;

        if (x.c[i].v != 1 || i == 0)
            fprintf(stream, "%" PRIu64, zp_as_int(x.c[i]));

        if (i > 0)
        {
            fputc('x', stream);
            if (i > 1)
                fprintf(stream, "^%zu", i);
        }
    }

    if (first)
        fprintf(stream, "%" PRIu64, zp_as_int(x.c[0]));
    else if (x.c[0].v)
        fprintf(stream, " + %" PRIu64, zp_as_int(x.c[0]));
}
