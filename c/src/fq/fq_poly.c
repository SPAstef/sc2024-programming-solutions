#include "fq/fq_poly.h"
#include "fq/fq.h"
#include "fq/fq_vec.h"
#include "utils.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

size_t fq_poly_maxdeg(fq_poly_t x)
{
    return x.n - 1;
}

size_t fq_poly_deg(fq_poly_t x)
{
    for (size_t i = fq_poly_maxdeg(x); i != 0; --i)
        if (!fq_is_zero(x.c[i]))
            return i;

    return 0;
}

size_t fq_poly_mindeg(fq_poly_t x)
{
    for (size_t i = 0; i < x.n; ++i)
        if (!fq_is_zero(x.c[i]))
            return i;

    return x.n;
}

fq_poly_t fq_poly_new_empty(size_t deg)
{
    return (fq_poly_t){.c = malloc((deg + 1) * sizeof(fq_t)), .n = deg + 1};
}

fq_poly_t fq_poly_new(size_t deg)
{
    fq_poly_t x = fq_poly_new_empty(deg);

    for (size_t i = 0; i <= deg; ++i)
        x.c[i] = fq_zero();

    return x;
}

fq_poly_t fq_poly_new_copy(fq_poly_t x)
{
    fq_poly_t y = fq_poly_new_empty(fq_poly_maxdeg(x));

    for (size_t i = 0; i < y.n; ++i)
        y.c[i] = fq_new_copy(x.c[i]);

    return y;
}

fq_poly_t fq_poly_new_view(fq_poly_t x)
{
    return x;
}

void fq_poly_del(fq_poly_t x)
{
    for (size_t i = 0; i < x.n; ++i)
        fq_del(x.c[i]);

    free(x.c);
}

void fq_poly_resize(fq_poly_t *x, size_t deg)
{
    size_t old_deg = fq_poly_maxdeg(*x);

    for (size_t i = deg + 1; i <= old_deg; ++i)
        fq_del(x->c[i]);

    x->c = realloc(x->c, (deg + 1) * sizeof(fq_t));
    x->n = deg + 1;

    for (size_t i = old_deg + 1; i <= deg; ++i)
        x->c[i] = fq_zero();
}

void fq_poly_copy(fq_poly_t *x, fq_poly_t y)
{
    fq_poly_del(*x);

    *x = fq_poly_new_copy(y);
}

void fq_poly_move(fq_poly_t *x, fq_poly_t y)
{
    fq_poly_del(*x);

    *x = y;
}

fq_poly_t fq_poly_zero()
{
    fq_poly_t x = fq_poly_new_empty(0);

    x.c[0] = fq_zero();

    return x;
}

fq_poly_t fq_poly_one()
{
    fq_poly_t x = fq_poly_new_empty(0);

    x.c[0] = fq_one();

    return x;
}

fq_poly_t fq_poly_self_add(fq_poly_t x, fq_poly_t y, uint64_t p)
{
    size_t d_x = fq_poly_deg(x);
    size_t d_y = fq_poly_deg(y);

    if (d_x < d_y)
    {
        fq_poly_resize(&x, d_y);
        d_x = d_y;
    }

    for (size_t i = 0; i <= d_y; ++i)
        x.c[i] = fq_self_add(x.c[i], y.c[i], p);

    return x;
}

fq_poly_t fq_poly_add(fq_poly_t x, fq_poly_t y, uint64_t p)
{
    return fq_poly_self_add(fq_poly_new_copy(x), y, p);
}

fq_poly_t fq_poly_self_neg(fq_poly_t x, uint64_t p)
{
    for (size_t i = 0; i < x.n; ++i)
        x.c[i] = fq_self_neg(x.c[i], p);

    return x;
}

fq_poly_t fq_poly_neg(fq_poly_t x, uint64_t p)
{
    return fq_poly_self_neg(fq_poly_new_copy(x), p);
}

fq_poly_t fq_poly_self_sub(fq_poly_t x, fq_poly_t y, uint64_t p)
{
    if (x.n < y.n)
        fq_poly_resize(&x, fq_poly_maxdeg(y));

    for (size_t i = 0; i < y.n; ++i)
        x.c[i] = fq_self_sub(x.c[i], y.c[i], p);

    return x;
}

fq_poly_t fq_poly_sub(fq_poly_t x, fq_poly_t y, uint64_t p)
{
    return fq_poly_self_sub(fq_poly_new_copy(x), y, p);
}

fq_poly_t fq_poly_self_mul(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p)
{
    size_t d_x = fq_poly_deg(x);
    size_t d_y = fq_poly_deg(y);
    fq_poly_t z = fq_poly_new(d_x + d_y);
    fq_t t = fq_zero();

    for (size_t i = 0; i <= d_x; ++i)
        for (size_t j = 0; j <= d_y; ++j)
        {
            fq_move(&t, fq_mul(x.c[i], y.c[j], r, p));
            z.c[i + j] = fq_self_add(z.c[i + j], t, p);
        }

    fq_poly_move(&x, z);
    fq_del(t);

    return x;
}

fq_poly_t fq_poly_mul(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p)
{
    return fq_poly_self_mul(fq_poly_new_copy(x), y, r, p);
}

fq_poly_t fq_poly_self_rem(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p)
{
    size_t d_x = fq_poly_deg(x);
    size_t d_y = fq_poly_deg(y);
    fq_t c = fq_zero();
    fq_t t = fq_zero();

    for (size_t i = d_x; i >= d_y; --i)
    {
        fq_move(&c, fq_div(x.c[i], y.c[d_y], r, p));

        for (size_t j = 0; j <= d_y; ++j)
        {
            fq_move(&t, fq_mul(y.c[d_y - j], c, r, p));
            x.c[i - j] = fq_self_sub(x.c[i - j], t, p);
        }
    }

    fq_del(c);
    fq_del(t);

    return x;
}

fq_poly_t fq_poly_rem(fq_poly_t x, fq_poly_t y, zp_poly_t r, uint64_t p)
{
    return fq_poly_self_rem(fq_poly_new_copy(x), y, r, p);
}

fq_t fq_poly_eval(fq_poly_t x, fq_t a, zp_poly_t r, uint64_t p)
{
    size_t d = fq_poly_maxdeg(x);
    fq_t y = x.c[d];

    for (size_t i = 1; i <= d; ++i)
        y = fq_self_add(fq_self_mul(y, a, r, p), x.c[d - i], p);

    return y;
}

bool fq_poly_is_irred(fq_poly_t x, zp_poly_t r, uint64_t p)
{
    uint64_t q = pow64(p, fq_poly_deg(x));
    fq_t a = fq_zero();

    for (uint64_t i = 0; i < q; ++i)
    {
        fq_move(&a, fq_from_int(i, p));
        fq_move(&a, fq_poly_eval(x, a, r, p));

        if (fq_is_zero(a))
            return false;
    }

    fq_del(a);

    return true;
}

fq_poly_t fq_poly_find_irred(zp_poly_t r, uint64_t p, size_t k)
{
    // k >= 2
    fq_poly_t x = fq_poly_new(k);

    fq_move(&x.c[0], fq_one());  // constant term must be non-zero
    fq_move(&x.c[k], fq_zero()); // enforce degree k

    do
        for (size_t i = 1; i < k; ++i)
            fq_move(&x.c[i], fq_rand(r, p));
    while (!fq_poly_is_irred(x, r, p));

    return x;
}

fq_poly_t fq_poly_from_fq_vec_view(fq_vec_t x)
{
    return (fq_poly_t){.c = x.c, .n = x.n};
}

fq_poly_t fq_poly_from_fq_vec_copy(fq_vec_t x)
{
    return fq_poly_from_fq_vec_view(fq_vec_new_copy(x));
}

fq_poly_t fq_poly_from_str(const char *str)
{
    return fq_poly_from_fq_vec_view(fq_vec_self_reverse(fq_vec_from_str(str)));
}

void fq_poly_print(FILE *stream, fq_poly_t x)
{
    bool first = true;

    for (size_t i = fq_poly_maxdeg(x); i > 0; --i)
    {
        if (fq_is_zero(x.c[i]))
            continue;

        if (!first)
            fprintf(stream, " + ");
        first = false;

        if (!fq_is_one(x.c[i]))
        {
            fputc('(', stream);
            fq_print(stream, x.c[i]);
            fputc(')', stream);
        }

        fputc('x', stream);
        if (i > 1)
            fprintf(stream, "^%zu", i);
    }

    if (first)
    {
        fputc('(', stream);
        fq_print(stream, x.c[0]);
        fputc(')', stream);
    }
    else if (!fq_is_zero(x.c[0]))
    {
        fprintf(stream, " + (");
        fq_print(stream, x.c[0]);
        fputc(')', stream);
    }
}
